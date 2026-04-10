# ============================================================================
# Vending Machine -- Demo
# ============================================================================
# Run this file to see the vending machine in action.
# All classes are in the vendingmachine/ package (one file per class),
# mirroring the Java repo structure from ByteByteGo.
# ============================================================================

from vendingmachine import Product, Rack, VendingMachine


def main():
    print("========== Vending Machine Simulation ==========\n")

    # ---- 1. Create products ----
    cola = Product("P001", "Cola", 1.50)
    chips = Product("P002", "Chips", 1.00)
    water = Product("P003", "Water", 0.75)
    candy = Product("P004", "Candy Bar", 2.00)

    # ---- 2. Create racks with inventory ----
    racks = {
        "A1": Rack("A1", cola, 5),
        "A2": Rack("A2", chips, 3),
        "B1": Rack("B1", water, 10),
        "B2": Rack("B2", candy, 2),
    }

    # ---- 3. Set up the vending machine ----
    vm = VendingMachine()
    vm.set_racks(racks)

    # ---- 4. Show initial state ----
    print(f"State: {vm.get_state_description()}\n")

    # ---- 5. Successful purchase (exact amount) ----
    print("--- Purchase 1: Cola with exact change ---")
    vm.insert_money(1.50)
    vm.select_product("A1")
    vm.confirm_transaction()
    print()

    # ---- 6. Successful purchase (with change) ----
    print("--- Purchase 2: Chips with $2.00 (expect $1.00 change) ---")
    vm.insert_money(2.00)
    vm.select_product("A2")
    vm.confirm_transaction()
    print()

    # ---- 7. Insufficient funds ----
    print("--- Purchase 3: Candy Bar with $1.00 (insufficient) ---")
    vm.insert_money(1.00)
    try:
        vm.select_product("B2")
    except RuntimeError as e:
        print(f"[ERROR] {e}")
    vm.cancel_transaction()
    print()

    # ---- 8. Select without inserting money ----
    print("--- Purchase 4: Select without money ---")
    try:
        vm.select_product("B1")
    except RuntimeError as e:
        print(f"[ERROR] {e}")
    print()

    # ---- 9. Buy last 2 candy bars, then try when out of stock ----
    print("--- Purchase 5 & 6: Buy all candy bars, then try out-of-stock ---")
    for i in range(2):
        vm.insert_money(2.00)
        vm.select_product("B2")
        vm.confirm_transaction()

    vm.insert_money(2.00)
    try:
        vm.select_product("B2")
    except RuntimeError as e:
        print(f"[ERROR] {e}")
    vm.cancel_transaction()
    print()

    # ---- 10. Transaction history ----
    print("--- Transaction History ---")
    for i, txn in enumerate(vm.get_transaction_history(), 1):
        print(f"  {i}. {txn}")

    print(f"\n========== Simulation Complete ==========")


if __name__ == "__main__":
    main()
