# ============================================================================
# Product
# ============================================================================
# Represents a basic product in the vending machine.
# Immutable by design -- product details (code, description, price) don't
# change once created.
#
# Note: Inventory quantity is NOT stored here. The product class only holds
# innate properties. Stock levels are managed by InventoryManager, since
# they change frequently (Single Responsibility Principle).
# ============================================================================


class Product:
    def __init__(self, product_code: str, description: str, unit_price: float):
        self.product_code = product_code
        self.description = description
        self.unit_price = unit_price

    def __repr__(self):
        return f"Product({self.product_code}, '{self.description}', ${self.unit_price:.2f})"
