BitShares Application for Nano S: Common Technical Specifications 
=================================================================

Christopher J. Sanborn _and_ Marcial Vieira

**Specification Version:**  0.1  _(DRAFT)_

## About

This application describes the APDU messages interface to communicate with the application. 

The application covers the following functionalities:

  - Retrieve a public key given a BIP 32 path
  - Sign a basic transaction given a BIP 32 path

The application interface can be accessed over HID

## General purpose APDUs

APDU requests sent to the device begin with an instruction class byte (CLA), followed by an instruction byte (INS), then two parameter bytes, (P1 and P2), and then variable length data.

All requests handled by the BitShares app described here will use CLA byte `0xB5`.  Instruction bytes handled by the app, and described in sections below, are:

|  INS  | Instruction    |
|:-----:|:---------------|
| `02`  | [Get Public Key](#get-public-key) |
| `04`  | [Sign BitShares Serialized Transaction](#sign-transaction) |
| `06`  | [Get App Configuration](#get-app-configuration) |

### GET PUBLIC KEY

#### Description

This command returns the public key and public key in WIF format for the given BIP 32 path.

The address can be optionally checked on the device before being returned.

#### Coding

##### _Command:_

| CLA | INS | P1 | P2 | Lc | Le |
|:---:|:---:|:---|:---|:--:|:--:|
|`B5` | `02`| `00`: return address immediately<br>`01`: display address and confirm before returning | `00`: do not return the chain code<br>`01`: return the chain code | variable | variable |

##### _Input data:_

| Description | Length |
|:------------|:------:|
| Number of BIP 32 derivations to perform (max 10)  |  1  |
| First derivation index (big endian)  |  4  |
| ...                                  |  4  |
| Last derivation index (big endian)   |  4  |

##### _Output data:_

| Description                          | Length |
|:-------------------------------------|:------:|
| Public Key length                    |   1    |
| Uncompressed Public Key              |  var   |
| BitShares WIF Public Key length      |   1    |
| BitShares WIF Public Key             |  var   |
| Chain code, if requested             |   32   |

##### _Example input and output:_

```
HID => b502000115058000003080000001800000018000000080000000
HID <= 41049c8313c7b70aa9ae85cdbe771533f670c67d8cd83c6049d218efae7c9ce4c6f3d5435ee9b6deb0da0db7de02d2c8f593a6e0
       5f64a9288e0b22b1a990447cd1b6354254533635524634427a686531624545355650314276754c7139786e35456671364b6a5073
       756f7a42746d6368477a6a6a617a41684d3f73fecfc00a0a156a1c7fc238fd1e65bc3dd0607674cdc9b48b095f0bc7fa9000
```

### SIGN TRANSACTION

#### Description

This command signs a BitShares transaction after having the user validate the following parameters:

  - Operation Name(s) (May be multiple operations in a transaction)
  - Operation Details for each operation in transaction
  - Transaction Id

The input data is the DER encoded transaction (each transaction field is encoded as StringOctet type), streamed to the device in 255 bytes maximum data chunks.

Data fields and the order used for signing:

  - chain id  
  - ref_block_num
  - ref_block_prefix
  - expiration
  - operation_list_size
  - operation_id
  - operation_data _(This is a single DER field.  We don't DER the individual fields of the operation.)_
  - _(id and data fields may be repeated if >1 operation)_
  - transaction_extensions_list_size

Field transaction_extensions_list_size should be 0 valued. Transaction extensions are not supported at this time.

#### Coding

##### _Command:_

|  CLA  |  INS   |  P1                |  P2        |  Lc   |  Le   |
|:-----:|:------:|:-------------------|:-----------|:-----:|:-----:|
| `B5`  |  `04`  |  `00`: first transaction data block<br>`80`: subsequent transaction data block | `00` | variable | variable |

##### _Input data (first transaction data block):_

| Description                                                                       | Length |
|:----------------------------------------------------------------------------------|:------:|
| Number of BIP 32 derivations to perform (max 10)                                  | 1
| First derivation index (big endian)                                               | 4
| ...                                                                               | 4
| Last derivation index (big endian)                                                | 4
| DER transaction chunk                                                             | variable

##### _Input data (other transaction data block):_

| Description                                                                       | Length
|:----------------------------------------------------------------------------------|:------:|
| DER transaction chunk                                                             | variable

##### _Output data:_

| Description                                                                       | Length |
|:----------------------------------------------------------------------------------|:------:|
| v                                                                                 | 1
| r                                                                                 | 32
| s                                                                                 | 32

### GET APP CONFIGURATION

#### Description

This command returns specific application configuration

#### Coding

##### _Command:_

|  CLA  |  INS   |  P1                |  P2        |  Lc   |  Le  |
|:-----:|:------:|:-------------------|:-----------|:-----:|:----:|
|  `B5` |  `06`  | `00`               | `00`       | `00`  | `04` |

##### _Input data:_

None

##### _Output data:_

| Description                                                                       | Length |
|:----------------------------------------------------------------------------------|:------:|
| Flags: `0x01`: arbitrary data signature enabled by user                           | 01 |
| Application major version                                                         | 01 |
| Application minor version                                                         | 01 |
| Application patch version                                                         | 01 |

## Transport protocol

### General transport description

Ledger APDUs requests and responses are encapsulated using a flexible protocol allowing to fragment large payloads over different underlying transport mechanisms. 

The common transport header is defined as follows: 

| Description                                                                       | Length |
|:----------------------------------------------------------------------------------|:------:|
| Communication channel ID (big endian)                                             | 2 |
| Command tag                                                                       | 1 |
| Packet sequence index (big endian)                                                | 2 |
| Payload                                                                           | var |

The Communication channel ID allows commands multiplexing over the same physical link. It is not used for the time being, and should be set to 0101 to avoid compatibility issues with implementations ignoring a leading 00 byte.

The Command tag describes the message content. Use TAG_APDU (0x05) for standard APDU payloads, or TAG_PING (0x02) for a simple link test.

The Packet sequence index describes the current sequence for fragmented payloads. The first fragment index is 0x00.

### APDU Command payload encoding

APDU Command payloads are encoded as follows :

| Description                                                                       | Length |
|:----------------------------------------------------------------------------------|:------:|
| APDU length (big endian)                                                          | 2 |
| APDU CLA                                                                          | 1 |
| APDU INS                                                                          | 1 |
| APDU P1                                                                           | 1 |
| APDU P2                                                                           | 1 |
| APDU length                                                                       | 1 |
| Optional APDU data                                                                | var |

APDU payload is encoded according to the APDU case 

| Case Number  |  Lc  |  Le  | Case description |
|:------------:|:----:|:----:|:-----------------|
|   1          |  0   |  0   | No data in either direction - L is set to 00 |
|   2          |  0   |  !0  | Input Data present, no Output Data - L is set to Lc |
|   3          |  !0  |  0   | Output Data present, no Input Data - L is set to Le |
|   4          |  !0  |  !0  | Both Input and Output Data are present - L is set to Lc |

### APDU Response payload encoding

APDU Response payloads are encoded as follows :

| Description                                                                       | Length |
|:----------------------------------------------------------------------------------|:------:|
| APDU response length (big endian)                                                 | 2 |
| APDU response data and Status Word                                                | var |

### USB mapping

Messages are exchanged with the dongle over HID endpoints over interrupt transfers, with each chunk being 64 bytes long. The HID Report ID is ignored.

## Status Words 

The following standard Status Words are returned for all APDUs - some specific Status Words can be used for specific commands and are mentioned in the command description.

##### _Status Words:_

|  SW      |  Description |
|:--------:|:-------------|
|   6700   | Incorrect length |
|   6985   | Security status not satisfied (Canceled by user) |
|   6A80   | Invalid data |
|   6B00   | Incorrect parameter P1 or P2 |
|   6Fxx   | Technical problem (Internal error, please report) |
|   9000   | Normal ending of the command |
