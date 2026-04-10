# ============================================================================
# Transaction
# ============================================================================
# A data structure that tracks the current state of an in-progress purchase.
# Encapsulates the selected product, the rack it belongs to, and the total
# cost/change for the purchase.
#
# This improves coordination between PaymentProcessor and InventoryManager --
# both can reference transaction details without being coupled to each other.
# ============================================================================

from typing import Optional
from vendingmachine.product import Product
from vendingmachine.rack import Rack


class Transaction:
    def __init__(self):
        self.rack: Optional[Rack] = None
        self.product: Optional[Product] = None
        self.total_amount: float = 0.0

    def __repr__(self):
        product_name = self.product.description if self.product else "None"
        return f"Transaction({product_name}, change=${self.total_amount:.2f})"
