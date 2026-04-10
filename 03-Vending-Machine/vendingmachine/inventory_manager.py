# ============================================================================
# InventoryManager
# ============================================================================
# Manages the collection of racks and handles inventory operations:
#   - Getting a product from a rack by rack code
#   - Dispensing a product (decrementing rack count)
#   - Updating the entire rack configuration (admin operation)
#
# Uses a dict (HashMap equivalent) for O(1) rack lookup by code.
# The dispense method interfaces with what would be hardware in a real system.
# ============================================================================

from typing import Optional
from vendingmachine.product import Product
from vendingmachine.rack import Rack


class InventoryManager:
    def __init__(self):
        self.racks: dict[str, Rack] = {}

    def update_racks(self, racks: dict[str, Rack]):
        self.racks = racks

    def get_rack(self, rack_code: str) -> Optional[Rack]:
        return self.racks.get(rack_code)

    def get_product_in_rack(self, rack_code: str) -> Optional[Product]:
        rack = self.racks.get(rack_code)
        if rack is None:
            return None
        return rack.get_product()

    def dispense_product_from_rack(self, rack: Rack):
        if rack.get_product_count() > 0:
            rack.count -= 1
        else:
            raise RuntimeError(f"Cannot dispense product. Rack {rack.rack_code} is empty.")
