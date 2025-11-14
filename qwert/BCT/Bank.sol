// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.19;

contract BankAccount {
    mapping(address => uint256) private balances;
    mapping(address => bool) private isUser;

    event AccountCreated(address indexed user, uint256 amount);
    event Deposit(address indexed user, uint256 amount);
    event Withdraw(address indexed user, uint256 amount);
    event Received(address indexed user, uint256 amount);

    // Create account and optionally deposit ETH
    function createAccount() public payable {
        require(!isUser[msg.sender], "Account exists");
        isUser[msg.sender] = true;
        balances[msg.sender] = msg.value;
        emit AccountCreated(msg.sender, msg.value);
    }

    // Deposit to your account via function call
    function deposit() public payable {
        require(isUser[msg.sender], "No account");
        require(msg.value > 0, "Amount > 0");
        balances[msg.sender] += msg.value;
        emit Deposit(msg.sender, msg.value);
    }

    // Allow plain transfers to this contract to be credited (e.g., send from wallet)
    receive() external payable {
        if (!isUser[msg.sender]) {
            isUser[msg.sender] = true; // optionally create account automatically
        }
        balances[msg.sender] += msg.value;
        emit Received(msg.sender, msg.value);
    }

    // Withdraw using Checks-Effects-Interactions and using call
    function withdraw(uint256 amount) public {
        require(isUser[msg.sender], "No account");
        require(balances[msg.sender] >= amount, "Low balance");

        // Effects
        balances[msg.sender] -= amount;

        // Interaction
        (bool success, ) = payable(msg.sender).call{value: amount}("");
        require(success, "Transfer failed");

        emit Withdraw(msg.sender, amount);
    }

    // Show your own balance
    function showBalance() public view returns (uint256) {
        require(isUser[msg.sender], "No account");
        return balances[msg.sender];
    }

    // Helper: view any user's balance (useful for testing)
    function getBalance(address user) public view returns (uint256) {
        return balances[user];
    }

    // Helper: check if address is registered
    function hasAccount(address user) public view returns (bool) {
        return isUser[user];
    }
}
