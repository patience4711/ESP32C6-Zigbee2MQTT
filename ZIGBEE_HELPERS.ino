
char *split(char *str, const char *delim)
{
    char *p = strstr(str, delim);

    if (p == NULL)
        return NULL; // delimiter not found

    *p = '\0';                // terminate string after head
    return p + strlen(delim); // return tail substring
}

void unjoinDevice(uint8_t devNumber) {

    esp_zb_zdo_mgmt_leave_req_param_t leave_req;
    memset(&leave_req, 0, sizeof(leave_req));
    
    // Vul het doeladres van de lamp in
    leave_req.dst_nwk_addr = Dev_Prop[devNumber].shortAddr;
    
// 4. GECORRIGEERD: Kopieer het lange IEEE adres direct vanuit je array met memcpy
    memcpy(leave_req.device_address, Dev_Prop[devNumber].devIeee, sizeof(esp_zb_ieee_addr_t));
    
    // Opties instellen:
    leave_req.remove_children = 0; // De lamp heeft zelf geen 'kinderen' (eindapparaten)
    leave_req.rejoin          = 0; // 0 = mag NIET automatisch direct herverbinden, moet clean gereset worden
    
    Serial.printf("send Leave Request to device: 0x%04X...\n", Dev_Prop[devNumber].shortAddr);
    
    // Verstuur het unjoin-commando via de SDK
    if (esp_zb_lock_acquire(portMAX_DELAY)) {
         esp_zb_zdo_device_leave_req(&leave_req, NULL, NULL);
         esp_zb_lock_release();
      } else {
        Serial.println("Error: could not get the Zigbee lock!");
      }

}
