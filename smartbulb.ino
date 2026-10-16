/*
        
readZB  FE0C 45CA 3143 7C826D8FAB38C1A4  00001B rc=17
short address 4331 and ieee A4C138AB8F6D827C

FE14670000FFFFCCDBA71699800000070903295F87E93143AB rc=25

endpoint reg 240001040100010004000006000800000303060008000003

working on /of 2401314301010600040100  0E03010000   
dim 50%        2401314301010800040100  06030104 7F0A00 50% = 0x7F so 100 is 254 0x00 … 0xFE   (0 … 254 decimal)
color temp     2401314301010003040100  0703010A 7201 0A00 warm white
               2401314301010003040100  0703010A FA00 0A00 neutral white
               2401314301010003040100  0703010A 9900 0A00 cool white
RGB            2401314301010003040100  08030107 2CB33F4C 0A00   red  xy method X, Y range: 0x0000 … 0xFEFF (0 … 65279)
               2401314301010003040100  08030107 2C23 BF7F 0A00   green
               2401314301010003040100  08030107 8F153A0A 0A00   blue


incoming mqtt from a smart bulb
{"Battery":255,"Color":null,"LastUpdate":"2026-01-07 19:16:36","Level":37,"RSSI":12,"description":"",
"dtype":"Color Switch","hwid":"5","id":"00082946","idx":946,"name":"TESTSBULB","nvalue":15,
"org_hwid":"5","stype":"RGB","svalue1":"37","switchType":"Dimmer","unit":1}
there is an idx 946
unknown device

pairing log 
extracted frame: FE0C45CA59C49BDEB1CC8C38C1A400
the comp value in frameHex is 45CA
â†’ found join frame
Device announce received (45CA)
New device joined: short=59C4 ieee=9BDEB1CC8C38C1A4
success, device got Address 59C4
saving the file /Dev_Prop0.str
sendZB FE04260836FFFC001F
Permit join disabled

length log: 838

*/


