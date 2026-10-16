/*
on switching i get 
FE03 45C4 CB06 004FFE1B448100000600CB060101006B00275A0B00000718F60A00001000CB061D2B   
and
FE03 45C4 CB06 004FFE1B448100000600 CB06 0101006B00275A0B00000718F60A00001000CB061D2B

working on command  2401CB06010106000401000E03010100
off                 2401CB06010106000401000E03010000

CB06  → short address
01    → destination endpoint (assumed)
01    → source endpoint
0600  → cluster On/Off
0401  → profile 0x0104
00    → options
0E    → radius
03    → data length
01    → frame control (cluster specific, client→server)
01    → command = ON
00    → transaction sequence
*/