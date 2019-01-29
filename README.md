# Bitshares Wallet App for Ledger Nano S

Bitshares wallet for Ledger Nano S.  This is the app that runs on the Nano S and signs transactions recieved via USB from a host wallet interface.  It also manages your keys, safeguarding the private keys and deriving the public and private keys from the wallet seed following [BIP-0032](https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki) and [SLIP-0048](https://github.com/satoshilabs/slips/blob/master/slip-0048.md).  A set of python scripts are provided to allow command-line signing of transactions where the transaction is read from a file in JSON format.  The main purpose of these scripts, aside from testing and verifying that the Nano app works as intended, is to provide a reference for developers developing GUI wallets.

The app follows the specification available in the doc/ folder, which documents the communication protocol with the device.

To use the generic wallet refer to `signTransaction.py`, `getPublicKey.py`.

### Features:

* Decodes parameters of recognized operations and displays them so that the user may be assured that the transaction is as intended.
* Can process transactions with up to four operations in the transaction.
* For unrecognized operations, displays a warning that the details cannot be extracted, but still allows the user to sign the transaction if they wish.

### Supported Operations:

* Transfer

# How to Install developer version
## Configuring Ledger Environment

* Install Vagrant and Virtualbox on your machine
* Run the following

```
git clone https://github.com/fix/ledger-vagrant
cd ledger-vagrant
vagrant up
```

This will take a few minutes to install

## Compile your ledger app

* install your app under apps/ for instance:
```
cd apps/
git clone https://github.com/christophersanborn/ledger-app-bitshares

```
* connect to the machine with `ssh vagrant`
* build eos app

```
cd apps/ledger-app-bitshares
make clean
make
```

* connect your ledger Nano S to your computer
* install the app on your ledger: `make load`
* remove the app from the ledger: `make delete`

Install instruction with slight modifications has been taken from [here](https://github.com/fix/ledger-vagrant)
