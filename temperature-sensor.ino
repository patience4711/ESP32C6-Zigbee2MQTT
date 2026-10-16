/* joningin
readZB FE0D45 C9FD 0A846523BD7238C1A400000026 rc=18

readZB FE0C45CA73DB846523BD7238C1A400007B rc=17
Device announce received (45CA)
sending pairing sequence
sendZB FE0A250573DBA4C13872BD236584D2
readZB FE0165050061 rc=6
sendSequence: no answer on request
Short Address: 73DB ??
IEEE Address: 846523BD7238C1A4
sendZB FE04260836FFFC001F
Permit join disabled
readZB FE016608006F rc=6

when it stopped working i did a register endpoint 2400010401000001010402010000

extracted frame: FE016608
the comp value in frameHex is 6608
extracted frame: FE0C45CAA766846523BD7238C1A400
the comp value in frameHex is 45CA
â†’ found join frame
Device announce received (45CA)
New device joined: short=A766 ieee=846523BD7238C1A4
success, device got Address A766
saving the file /Dev_Prop0.str
sendZB FE04260836FFFC001F
Permit join disabled

length log: 534
*/