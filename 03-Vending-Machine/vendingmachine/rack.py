# ============================================================================
# Rack
# ============================================================================
# Models a single rack/slot in the vending machine.
# Each rack holds multiple units of a SINGLE product type.
#
# The Rack class only manages its own count and product reference.
# Dispensing logic is delegated to InventoryManager, following the
# Single Responsibility Principle.
# ============================================================================

from vendingmachine.product import Product


class Rack:
    def __init__(self, rack_code: str, product: Product, count: int):
        self.rack_code = rack_code
        self.product = product
        self.count = count

    def get_product(self) -> Product:
        return self.product

    def get_product_count(self) -> int:
        return self.count

    def __repr__(self):
        return f"Rack({self.rack_code}, {self.product.product_code}, qty={self.count})"
