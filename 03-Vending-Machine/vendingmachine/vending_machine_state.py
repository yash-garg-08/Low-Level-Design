# ============================================================================
# VendingMachineState (State Pattern) and Concrete States
# ============================================================================
# The State Pattern enforces a strict sequence of user actions:
#   Insert Money -> Select Product -> Dispense Product
#
# Each state defines which actions are allowed and which raise errors:
#
# - NoMoneyInsertedState:
#     Only insertMoney() is allowed. Selecting or dispensing raises an error.
#     Transitions to -> MoneyInsertedState on success.
#
# - MoneyInsertedState:
#     Only selectProduct() is allowed. Validates stock and sufficient funds.
#     Transitions to -> DispenseState on success.
#
# - DispenseState:
#     Only dispenseProduct() is allowed. Dispenses item, returns change.
#     Transitions to -> NoMoneyInsertedState (reset for next customer).
#
# This prevents invalid flows like dispensing without payment or selecting
# a product before inserting money.
# ============================================================================

from __future__ import annotations
from abc import ABC, abstractmethod
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from vendingmachine.vending_machine import VendingMachine

from vendingmachine.transaction import Transaction


class VendingMachineState(ABC):
    @abstractmethod
    def insert_money(self, vm: VendingMachine, amount: float):
        pass

    @abstractmethod
    def select_product(self, vm: VendingMachine, rack_code: str):
        pass

    @abstractmethod
    def dispense_product(self, vm: VendingMachine):
        pass

    @abstractmethod
    def get_state_description(self) -> str:
        pass


class NoMoneyInsertedState(VendingMachineState):
    """Initial state. User must insert money before doing anything else."""

    def insert_money(self, vm: VendingMachine, amount: float):
        vm.payment_processor.add_balance(amount)
        print(f"[INSERT] ${amount:.2f} inserted. Balance: ${vm.payment_processor.get_current_balance():.2f}")
        vm.set_state(MoneyInsertedState())

    def select_product(self, vm: VendingMachine, rack_code: str):
        raise RuntimeError("Cannot select a product without inserting money first.")

    def dispense_product(self, vm: VendingMachine):
        raise RuntimeError("Cannot dispense product without inserting money first.")

    def get_state_description(self) -> str:
        return "No Money Inserted - Please insert money to proceed"


class MoneyInsertedState(VendingMachineState):
    """Money has been inserted. User must select a product."""

    def insert_money(self, vm: VendingMachine, amount: float):
        vm.payment_processor.add_balance(amount)
        print(f"[INSERT] ${amount:.2f} added. Balance: ${vm.payment_processor.get_current_balance():.2f}")

    def select_product(self, vm: VendingMachine, rack_code: str):
        rack = vm.inventory_manager.get_rack(rack_code)
        if rack is None:
            raise RuntimeError(f"Invalid rack code: {rack_code}")

        product = rack.get_product()
        if rack.get_product_count() == 0:
            raise RuntimeError(f"Product '{product.description}' is out of stock.")

        if vm.payment_processor.get_current_balance() < product.unit_price:
            raise RuntimeError(
                f"Insufficient funds. Need ${product.unit_price:.2f}, "
                f"have ${vm.payment_processor.get_current_balance():.2f}"
            )

        vm.current_transaction.product = product
        vm.current_transaction.rack = rack
        print(f"[SELECT] Selected '{product.description}' (${product.unit_price:.2f}) from rack {rack_code}")
        vm.set_state(DispenseState())

    def dispense_product(self, vm: VendingMachine):
        raise RuntimeError("Please select a product first.")

    def get_state_description(self) -> str:
        return "Money Inserted - Please select a product"


class DispenseState(VendingMachineState):
    """Product selected. Ready to dispense."""

    def insert_money(self, vm: VendingMachine, amount: float):
        raise RuntimeError("Cannot insert money during dispensing. Please wait.")

    def select_product(self, vm: VendingMachine, rack_code: str):
        raise RuntimeError("Cannot change selection during dispensing. Please wait.")

    def dispense_product(self, vm: VendingMachine):
        txn = vm.current_transaction

        vm.payment_processor.charge(txn.product.unit_price)
        vm.inventory_manager.dispense_product_from_rack(txn.rack)

        change = vm.payment_processor.return_change()
        txn.total_amount = change

        print(f"[DISPENSE] Dispensed '{txn.product.description}'", end="")
        if change > 0:
            print(f" | Change: ${change:.2f}", end="")
        print()

        vm.transaction_history.append(txn)
        vm.current_transaction = Transaction()
        vm.set_state(NoMoneyInsertedState())

    def get_state_description(self) -> str:
        return "Dispensing product..."
