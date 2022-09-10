# BWRC-ChipUtil-TileLinkTrafficGenerator

[TileLink Spec v1.8.1](https://starfivetech.com/uploads/tilelink_spec_1.8.1.pdf)

## TileLink

(OSCIBEAR Setup)

### A Channel

| Signal    | Type    | Width | Description |
| --------- | ------- | ----- | -------- |
| a_opcode  | C       | 3       | |
| a_param   | C       | 3       | |
| a_size    | C       | z = 4   | |
| a_source  | C       | o = 4   | |
| a_address | C       | a = 32  | |
| a_mask    | C       | w = 8   | |
| a_data    | D       | 8w = 64 | |
| a_corrupt | D       | 1       | |
| a_valid   | V       | 1       | |
| a_ready   | R (inv) | 1       | |

### Messages

| Type            | Channel | Opcode | Param | Note                           |
| --------------- | ------- | ------ | ----- | ------------------------------ |
| PutFullData     | Ch. A   | 0x0    | 0x0   | Slave responses AccessAck      |
| PutPartialData  | Ch. A   | 0x1    | 0x0   | Slave responses AccessAck      |
| Get             | Ch. A   | 0x4    | 0x0   | Slave responses AccessAckData  |
| AccessAck       | Ch. D   | 0x0    | 0x0   |                                |
| AccessAckData   | Ch. D   | 0x1    | 0x0   |                                |


### Example Frames

Writing to OSCIBEAR DTIM memory @ 0x8000_0000 with data 0x0000_0000_0000_0001

A channel PutFullData operation

```
| chanid    | a_opcode  | a_param   | size          | source        | address                                 | mask      | data                                                                            | corrupt | last     |
| 0   0   0 | 0   0   0 | 0   0   0 | 0   0   1   0 | 0   0   0   0 | 1000 0000 0000 0000 0000 0000 0000 0000 | 0000 1111 | 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0001 | 0       | 1        |
| CHANNEL A |PutFullData|PutFullData| 2**2 = 4 bytes| source id 0   | 0x8000 0000                             | uint32_t  | 0x0000 0000 0000 0001                                                           |         | always 1 |
```

In reverse order:
```
000000000001000001000000000000000000000000000000000001111000000000000000000000000000000000000000000000000000000000000000101
```



RX packet:
```
100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000110
```

In reverse:
011000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001

```
| chanid    | a_opcode  | a_param   | size          | source        | address                                 | mask      | data                                                                            | corrupt | last     |
| 0   0   0 | 0   0   0 | 0   0   0 | 0   0   1   0 | 0   0   0   0 | 1000 0000 0000 0000 0000 0000 0000 0000 | 0000 1111 | 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0001 | 0       | 1        |
| CHANNEL A |PutFullData|PutFullData| 2**2 = 4 bytes| source id 0   | 0x8000 0000                             | uint32_t  | 0x0000 0000 0000 0001                                                           |         | always 1 |
```

