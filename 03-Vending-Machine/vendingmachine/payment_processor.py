# ============================================================================
# PaymentProcessor
# ============================================================================
# Handles all payment-related operations:
#   - Adding funds (user inserts cash)
#   - Charging for a purchase (deducting product price)
#   - Returning change (whatever balance remains after charge)
#
# In a real system, this would interface with coin receptacles or card
# readers. Here we model it as a simple balance tracker.
# No dependencies on other domain classes (isolated responsibility).
# ============================================================================


class PaymentProcessor:
    def __init__(self):
        self.current_balance: float = 0.0

    def add_balance(self, amount: float):
        self.current_balance += amount

    def charge(self, amount: float):
        self.current_balance -= amount

    def return_change(self) -> float:
        change = self.current_balance
        self.current_balance = 0.0
        return change

    def get_current_balance(self) -> float:
        return self.current_balance
