# ============================================================================
# VendingMachine (Facade Pattern)
# ============================================================================
# The central entry point for all user interactions. Delegates to:
#   - InventoryManager for stock management
#   - PaymentProcessor for cash handling
#   - VendingMachineState for enforcing action sequences
#
# Keeps the facade lightweight by only coordinating -- it doesn't contain
# business logic for inventory, payment, or state transitions itself.
#
# Admin operations (set_racks) are separated from user operations
# (insert_money, select_product, confirm_transaction) for security.
# ============================================================================

from vendingmachine.inventory_manager import InventoryManager
from vendingmachine.payment_processor import PaymentProcessor
from vendingmachine.transaction import Transaction
from vendingmachine.rack import Rack
from vendingmachine.vending_machine_state import VendingMachineState, NoMoneyInsertedState


class VendingMachine:
    def __init__(self):
        self.inventory_manager = InventoryManager()
        self.payment_processor = PaymentProcessor()
        self.current_transaction = Transaction()
        self.transaction_history: list[Transaction] = []
        self._state: VendingMachineState = NoMoneyInsertedState()

    def set_state(self, state: VendingMachineState):
        self._state = state

    def get_state_description(self) -> str:
        return self._state.get_state_description()

    # --- Admin operations ---

    def set_racks(self, racks: dict[str, Rack]):
        self.inventory_manager.update_racks(racks)

    # --- User operations (delegated to current state) ---

    def insert_money(self, amount: float):
        self._state.insert_money(self, amount)

    def select_product(self, rack_code: str):
        self._state.select_product(self, rack_code)

    def confirm_transaction(self):
        self._state.dispense_product(self)

    def cancel_transaction(self):
        change = self.payment_processor.return_change()
        if change > 0:
            print(f"[CANCEL] Transaction cancelled. Returned ${change:.2f}")
        else:
            print("[CANCEL] Transaction cancelled.")
        self.current_transaction = Transaction()
        self.set_state(NoMoneyInsertedState())

    def get_transaction_history(self) -> list[Transaction]:
        return list(self.transaction_history)
