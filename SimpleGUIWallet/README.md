## SimpleGUIWallet — Minimalistic Wallet for Hardware-secured BitShares Accounts

_SimpleGUIWallet_ is a minimalistic wallet interface for using the Ledger Nano S hardware wallet to sign transactions for BitShares accounts that have been configured with keys stored on a Ledger Nano device.  It is a "companion app" to provide a graphical user interface (GUI) to the BitShares App running on the Ledger Nano device itself.

It is a minimalistic wallet intended to support primarily simple transfer operations, although it does provide a "Raw Transactions" facility for technically-inclined users who wish to sign arbitrary transactions with the Nano.

## Installation:

_SimpleGUIWallet_ is a Python app that can be easily run via a Python interpreter in Windows, MacOS, or Linux (including Raspbian).

**Note:** A "compiled app" may be available for your platform, for users who just want a simple, double-clickable icon.  Check _**[Releases (ledger-app-bitshares)](https://github.com/bitshares/ledger-app-bitshares/releases)**_ to see if your platform is listed.  

If a precompiled app is not available, then the following instructions should get _SimpleGUIWallet_ up and running on your system.  The instructions assume a basic familiarity with the command line.

#### Requirements:

* A Python 3 interpreter

  * Under most Linux distributions, Python 3 is likely available by default.
  * Under MacOS, however, the Python version might be Python 2, which won't work.  Tutorials for how to install Python 3 are available on the web.  A recent one I found is here: https://wsvincent.com/install-python3-mac/
  * Installation instructions for various OS platforms, including Windows, are avaialble here: https://realpython.com/installing-python/

#### Step-by-step:

First, clone the `ledger-app-bitshares` repository:

```
$ git clone https://github.com/bitshares/ledger-app-bitshares.git
$ cd ledger-app-bitshares
```

The _SimpleGUIWallet_ companion app is in a subfolder of the full `ledger-app-bitshares` repo:

```
$ cd SimpleGUIWallet/
```

##### Install dependencies:

_SimpleGUIWallet_ depends on several packages, listed in `requirements.txt`, from the PyPI repository for such things as GUI widgets, Nano communication, and interaction with the BitShares network.  Install these dependencies with:

```
$ pip3 install --user -r requirements.txt
```

_SimpleGUIWallet_ is now ready to run!

## Usage:

From the SimpleGUIWallet folder, start the companion app with:

```
$ python3 SimpleGUIWallet.py
```

Note that _SimpleGUIWallet_ does not save any state between sessions.  It will not remember your account name or your account history or balances (the blockchain does this for you!).  Each time you start the wallet, you will need to enter your account name on the line "BitShares User Account", and click "Refresh Balances".  To automate this process, you can start the app with a command-line switch:

```
$ python3 SimpleGUIWallet.py --sender your-account-here
```

(You can easily make a shell script (or "batch file") to append this argument for you, if desired.)

### Account Balances and History:

The tabs on the left side of the window list your account's balances ("Assets" tab) and your account's recent history ("History" tab).  They may not alway auto-update.  You can always refresh them with the "Refresh Balances" button.

### Simple Transfers:

_SimpleGUIWallet_ is intended to make it easy to do "the simple stuff" with your Nano-secured BitShares account.  It provides a rudimentary interface for basic transfers.  (And admittedly, it does not provide much else.)   Click the "Transfers" tab, and fill out each of:

* The recipient
* The amount
* The asset symbol

And click "Send Transfer" (after plugging in your Nano device and starting the BitShares app) to generate the Transfer transaction, send it to the Nano for confirmation and signing, and broadcast it to the network.  Note that you can click an asset in the "Assets" tab to auto-populate the Asset field.

The activity panel at the bottom of the window will give feedback as to the progress and success of each step.

### Account Keys:

The idea of _SimpleGUIWallet_ is that your account's private keys are stored only on the Ledger Nano.  _SimpleGUIWallet_ does NOT store or generate keys.  However, it will let you browse the public keys stored on your Nano, and select which one (identified by a "SLIP-48 path") will be used to sign transactions.

To browse public keys, go to the "Public Keys" tab.  The three lists cover the three "account roles" that define BitShares authorities.  The lists start out showing only the derivation paths.  If you wish to see the actual keys, connect your Nano and click "Query Addresses".  This will retrieve each key from the device.  Selecting one from the list boxes will print the key in the PubKey box at the top of the window, where you can copy-and-paste it elsewhere (e.g. when assigning those keys as authorities on the account).

Note that in general, you do not need to retrieve keys from the Nano on a routine basis.  All you need to do is specify which path to use, and the Nano will sign with the corresponding key (and the default path is the correct one for typical usage).  You only need to retrieve keys when first setting up an account to be controlled by the Nano.

_A tutorial for how to set up a BitShares account to be controlled by your Ledger Nano can be found here:_

* **[Securing BitShares with Ledger Nano (how.bitshares.works)](https://how.bitshares.works/en/master/user_guide/ledger_nano.html)**

**Important:** before copying a key from your Nano to assing to your account, be sure to click the "Confirm Address" button to view the key on your Nano's screen to ensure that the key on the device matches the key as reported by _SimpleGUIWallet_.

### Advanced Operations:

If you know how to construct the JSON represention of BitShares transactions, you can use the "Raw Transactions" tab to send any arbitrary transaction to the Nano for signing and broadcasting.  This tab isn't necessarily easy to use... but it makes most anything "possible".
