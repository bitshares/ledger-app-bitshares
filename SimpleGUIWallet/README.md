### SimpleGUIWallet

# A Minimalist Wallet for Hardware-secured BitShares Accounts

_SimpleGUIWallet_ is a minimalist wallet interface for using the Ledger Nano S hardware wallet on the BitShares network.  It's function is to communicate with the Nano device so that the device may sign transactions on behalf of BitShares accounts that have been authorized with keys stored on and managed by a Ledger Nano device.  It is a "companion app" that provides a graphical user interface (GUI) to the BitShares App running within the Ledger Nano device.

_SimpleGUIWallet_ is a minimalist wallet, intended primarily to support simple transfer operations, although it does also provide a "Raw Transactions" facility for technically-inclined users who wish to sign arbitrary transactions with their device.

## Installation:

_SimpleGUIWallet_ is a Python app that can be easily run via a Python interpreter in Windows, MacOS, or Linux, including Raspbian
on the Raspberry Pi.

On some platforms, an app bundle may be available for your platform's app menu or launcher system.  Check _**[Releases](https://github.com/bitshares/ledger-app-bitshares/releases)**_ to see if your platform is listed.  If not, then follow the instructions below to acquire and run _SimpleGUIWallet_ from a terminal or command line.

### Requirements:

* A Python3 interpreter, version 3.6 or greater

  * Under most Linux distributions, Python 3 is likely available by default, although on older systems you may need to update to get version 3.6 or greater.
  * Under MacOS, however, the Python version might be Python 2, which won't work.  Tutorials for how to install Python 3 are available on the web.  A recent one I found is here: https://wsvincent.com/install-python3-mac/
  * Installation instructions for various OS platforms, including Windows, are available here: https://realpython.com/installing-python/

* On Linux systems, you may need to allow USB acccess to the Nano by modifying your system's "udev-rules."  More information may be found here:

  * https://support.ledger.com/hc/en-us/articles/115005165269-Fix-connection-issues

### Step-by-step:

First, clone the `ledger-app-bitshares` repository:

```
git clone https://github.com/bitshares/ledger-app-bitshares.git
```

The _SimpleGUIWallet_ companion app is in a subfolder:

```
cd ledger-app-bitshares/SimpleGUIWallet/
```

### Dependencies:

_SimpleGUIWallet_ depends on several packages, from the PyPI repository, as listed in `requirements.txt`.  These dependencies enable the GUI widgets, Nano communication, and interaction with the BitShares network.  These dependencies will be installed automatically if you start the wallet with the `start-wallet.sh` script.

## Usage:

Start the wallet with:

```
./start-wallet.sh
```

Note that _SimpleGUIWallet_ does not save any state between sessions.  It will not remember your account name or your account history or balances (the blockchain does this for you!).  Each time you start the wallet, you will need to enter your account name on the line "BitShares User Account", and click "Refresh Balances".  To automate this process, you can start the app with a command-line switch:

```
./start-wallet.sh --user your-account-here
```

Or, you can edit the `start-wallet.sh` script and add the `--user your-account-name` to the last line that reads:
```
python3 SimpleGUIWallet.py "$@"
```

### Account Balances and History:

The Assets and History tabs on the left side of the window list your account's balances and recent history respectively.  The app does not auto-update balances; to refresh them, use the "Refresh Balances" button.

### Simple Transfers:

The app provides a rudimentary interface for basic transfers.  Click the "Transfers" tab, and fill out each of:

* The "Send To" Recipient
* The Asset Amount
* The Asset Symbol

For convenience, the UI allows you to click on an asset in the "Assets" tab to auto-populate the Asset Symbol field.

Next, plug in your Nano device, log into the Nano device with your Pin Code, and start the BitShares app.

To finalize the transaction click "Send Transfer".  _SimpleGUIWallet_ sends the transaction to the Nano device whereupon you will be able to view the details on the Nano screen.  You will be prompted to confirm the transaction on your Nano device.   After you confirm, the Nano signs the transaction and sends the signature back to _SimpleGUIWallet_, which will then broadcast it to the network over a public API node.

The activity panel at the bottom of the window will give feedback as to the progress and success of each step.

**Note:** Not all asset ID codes are recognized by the Nano firmware, due to app data storage restrictions, and the inability of the device to query the BitShares network.  Many popular tokens will be recognized and displayed to the user by name, but some tokens will only be displayable by their asset ID code, which may look strange to the user.  When SimpleGUIWallet seeks confirmation on an unrecognized asset_name, the Nano device will display the asset_id and "graphene integer amount", without the decimal place marked.  It is up to the user to verify that the asset ID's displayed on the Nano screen correspond to the token the user is intending to send.

### Account Keys:

The purpose of _SimpleGUIWallet_ is to store your account's private keys only on the Ledger Nano;  _SimpleGUIWallet_ does NOT store or generate keys.  However, it will let you browse the public keys stored on your Nano, and select which one will be used to sign transactions.  These are identified by their "SLIP-48 path".  SLIP-48 is described in detail [here](https://github.com/satoshilabs/slips/blob/master/slip-0048.md), and is similar in concept to Bitcoin's BIP-44.

To browse public keys, go to the "Public Keys" tab.  Three lists appear covering the three "account roles" that define BitShares authorities.  The lists initialize by displaying only the derivation paths.  If you wish to see the actual keys, connect your Nano and click "Query Addresses".  This will retrieve each key from the device.  Selecting one from the list boxes will print the key in the PubKey box at the top of the window.  Here, you can copy-and-paste it elsewhere (e.g. when assigning those keys as authorities on the account).

Note: You do not need to retrieve keys from the Nano on a routine basis.  All you need to do is specify which path to use, and the Nano will sign with the corresponding key.  The default path is generally the correct one for typical usage.  You only need to retrieve keys when first setting up an account to be controlled by the Nano.

_A tutorial for how to set up a BitShares account to be controlled by your Ledger Nano can be found here:_

* **[Securing BitShares with Ledger Nano (docs.bitshares.eu)](https://docs.bitshares.eu/en/master/user_guide/ledger_nano.html)**

**Important:** before copying a key from your Nano device to assign to your BitShares Account, be sure to click the "Confirm Address" button on the Public Keys tab. Doing so will allow you to view the key on your Nano's screen to ensure that the key on the device matches the key as reported by _SimpleGUIWallet_.

### Advanced Operations:

If you know how to construct the JSON representation of BitShares transactions, you can use the "Raw Transactions" tab to send any arbitrary transaction to the Nano for signing and broadcasting.  This tab isn't necessarily easy to use, but it makes almost any BitShares transaction "possible".
