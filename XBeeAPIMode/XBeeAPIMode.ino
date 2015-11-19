#include <XBee.h>
#include <SoftwareSerial.h>

/*
This example is for Series 2 XBee
*/

// Should be a unique ID between 0 and 255.
uint8_t BEACON_ID = 1;


XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

uint8_t dbCommand[] = {'D','B'};
AtCommandRequest atRequest = AtCommandRequest(dbCommand);

ZBTxStatusResponse txStatus = ZBTxStatusResponse();
AtCommandResponse atResponse = AtCommandResponse();

SoftwareSerial xbeeSerial(2,3);

void setup() {  
  // start serial
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
  Serial.println("Initializing beacon...");
}

void sendValue(XBeeAddress64 targetAddress, int Value){
  uint8_t value = (uint8_t) Value;
  uint8_t values[] = {value, BEACON_ID};
  ZBTxRequest zbTx = ZBTxRequest(targetAddress, values, sizeof(values));
  sendTx(zbTx);
}

void sendTx(ZBTxRequest zbTx){
  xbee.send(zbTx);

   if (xbee.readPacket(500)) {
    Serial.println("Got a response!");
    // got a response!

    // should be a znet tx status              
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success.  time to celebrate
        Serial.println("SUCCESS!");
      } else {
        Serial.println("FAILURE!");
        // the remote XBee did not receive our packet. is it powered on?
      }
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("Error reading packet.  Error code: ");  
    Serial.println(xbee.getResponse().getErrorCode());
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
    Serial.println("This should never happen...");
  }
}

void processResponse()
{
  if (xbee.getResponse().isAvailable()) 
  {
    // got something
           
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) 
    {
      // got a zb rx packet
        
      // now fill our zb rx class
      xbee.getResponse().getZBRxResponse(rx);
      
      Serial.println("Got an rx packet!");
            
      if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) 
      {
        // the sender got an ACK
        Serial.println("packet acknowledged");
      } 
      else 
      {
        Serial.println("packet not acknowledged");
      }
        
      Serial.print("checksum is ");
      Serial.println(rx.getChecksum(), HEX);

      Serial.print("packet length is ");
      Serial.println(rx.getPacketLength(), DEC);
        
      for (int i = 0; i < rx.getDataLength(); i++) 
      {
        Serial.print("payload [");
        Serial.print(i, DEC);
        Serial.print("] is ");
        Serial.println(rx.getData()[i], HEX);
      }
        
      for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) 
      {
        Serial.print("frame data [");
        Serial.print(i, DEC);
        Serial.print("] is ");
        Serial.println(xbee.getResponse().getFrameData()[i], HEX);
      }

            
      //XBeeAddress64 replyAddress = XBeeAddress64(0x00000000, 0x0000ffff);
      //int n = 233;
      //sendValue(replyAddress,n);
      Serial.println("");
        
      }
    } 
    else if (xbee.getResponse().isError()) 
    {
      Serial.print("error code:");
      Serial.println(xbee.getResponse().getErrorCode());
    }
}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {
  
    XBeeAddress64 address = XBeeAddress64(0x00000000, 0x0000ffff);
    
    int n = 505;
    sendValue(address,n);
  
    xbee.readPacket();
    processResponse();
}
