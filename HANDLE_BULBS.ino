void handleColor(AsyncWebServerRequest *request) 
{

    // this function handles the values coming frem the sliders
    // they are saved in the Dev_data struct.
    // Veiligheid voorop: controleer of het request-object bestaat
    if (request == nullptr) return;

    // Veilige startmelding zonder request->url()
    

    consoleOut("handleColor");

    // Start met de huidige waarden van het geselecteerde apparaat als fallback
    uint8_t RED = lampState[devChoice].present[R];
    uint8_t GRE = lampState[devChoice].present[G];
    uint8_t BLU = lampState[devChoice].present[B];

    // Haal de parameters handmatig op MET 'const' om de compiler tevreden te houden
    if (request->hasParam("RED")) {
        const AsyncWebParameter* p = request->getParam("RED"); // Nu met const!
        if (p != nullptr) {
            RED = p->value().toInt();
           consoleOut("found RED : " + String(RED));
        }
    }
    if (request->hasParam("GRE")) {
        const AsyncWebParameter* p = request->getParam("GRE"); // Nu met const!
        if (p != nullptr) {
            GRE = p->value().toInt();
            //Serial.printf("   Gevonden GRE: %d\n", GRE);
            consoleOut("found GRE : " + String(GRE));
        }
    }
    if (request->hasParam("BLU")) {
        const AsyncWebParameter* p = request->getParam("BLU"); // Nu met const!
        if (p != nullptr) {
            BLU = p->value().toInt();
            //Serial.printf("   Gevonden BLU: %d\n", BLU);
            consoleOut("found BLU : " + String(BLU));
        }
    }

    // Schrijf de waarden in de struct
    lampState[devChoice].present[R] = RED;
    lampState[devChoice].present[G] = GRE;
    lampState[devChoice].present[B] = BLU;
    lampState[devChoice].mode = 5; // set color mode
    pendingFlag = 2;
}
void handleDim(AsyncWebServerRequest *request) 
{
    // this function is called by a slider
    consoleOut("handleDim");
    if (!request->hasParam("level")) {
        consoleOut("Error: 'level' parameter ontbreekt in request!");
        request->send(400, "text/plain", "Bad Request: Missing level");
        return;
    }
    int level = request->getParam("level")->value().toInt();
    
    if (devChoice < 0 || devChoice >= devCount) { 
        consoleOut("Error: devChoice is out of bounds!");
        request->send(400, "text/plain", "Bad Request: Invalid device");
        return;
    }

    lampState[devChoice].present[D] = level;
    pendingFlag = 3;  
}





