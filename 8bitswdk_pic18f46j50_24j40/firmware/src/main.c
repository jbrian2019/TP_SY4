/**
 * TP SY34 
 *
 * Nom binôme :
 *
 * Version :
 *
 */






/************************ HEADERS ****************************************/
#include "VT100.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"


/************************** PROTOTYPES ************************************/





/************************** VARIABLES ************************************/

extern API_UINT16_UNION myPANID; // mon PANID
extern uint8_t myLongAddress[MY_ADDRESS_LENGTH]; // mon adresse IEEE
extern API_UINT16_UNION myShortAddress; // mon adresse logique
extern ACTIVE_SCAN_RESULT ActiveScanResults[ACTIVE_SCAN_RESULT_SIZE]; // table des actives scan

#define  CHANNEL 10
#define  RX_ADRESS 0x0100
char myPseudo;
uint8_t respondSearch;
uint8_t nbreAppui;


extern RECEIVED_MESSAGE rxMessage;

void chatInit(void) {
    //todo 
}

void getPseudo(char*pseudo){
   int	i=0;
	do{
		if(uartIsChar()){
		pseudo[i++] = uartRead();	
        }
	} while((pseudo[i-1]!=0x0D)|| (i>=9));
	pseudo[i-1] = 0;
}
void main(void) {
    SYSTEM_Initialize();
    uartInitialize();
    vT100tEraseScreen();
    vT100SetCursorPos(0,0);
    
    
    _uartPrint("Saisissez votre pseudo\r\n");
    getPseudo(myPseudo);
    _uartPrint("\r\n Bonjour : ");
    _uartPrint(myPseudo);
    _uartPrint("\r\n");
    MiApp_ProtocolInit(false);
    if (MiApp_SetChannel(CHANNEL)== false){  
    }
       respondSearch= MiApp_SearchConnection(30,CHANNEL);
       if(respondSearch!=0){
           uint8_t i ;
           uint8_t ActiveScanIndex ;
           for (i=0; i<=respondSearch; i++){
               if(ActiveScanResults[i].PANID.Val == MY_PAN_ID ){
                   ActiveScanIndex=i;
               }     
           }
           MiApp_ConnectionMode(ENABLE_ACTIVE_SCAN_RSP);
           MiApp_EstablishConnection(ActiveScanIndex,CONN_MODE_DIRECT);
           _uartPrint("connexion réussie sur PAN existant\n\r");
       }
       else{
           MiApp_ConnectionMode(ENABLE_ALL_CONN);
           MiApp_StartConnection(START_CONN_DIRECT,0,0);
           _uartPrint("Creation d'un nouveau PAN\n\r");
       }
       _uartPrint("votre adresse est : 0x");
        uartHexaPrint((uint8_t *)&myShortAddress,2);
        uartPrint("\r\n");
      while (1){
          TX();
          RX();
      };
}
char sendIncrement(){
            nbreAppui++;
            char charnbreAppui[] ="00";
            charnbreAppui[1]+= nbreAppui/10;
            charnbreAppui[2]+= nbreAppui%10;
            return charnbreAppui;
        } 
    RX(void){
    if(MiApp_MessageAvailable()){
       _uartPrint(rxMessage.Payload);
       MiApp_DiscardMessage();
    }
}
    TX(){
     MiApp_FlushTx();
    // MiApp_WriteData(0x55);//report type
     char chaine1 = "votre pseudo:";
     char chaine2= myPseudo;
     char chaine3= "message broadcast";
     char chaine4= sendIncrement();
     char chaine5= "0x0100";
     char chaine6= "vers";
     if( PORTBbits.RB0 !=0){
     MiApp_WriteData(chaine1);
     MiApp_WriteData(chaine2);
     MiApp_WriteData(chaine3);
     MiApp_WriteData(chaine4);
     MiApp_WriteData(0);
     
     MiApp_BroadcastPacket(true);
     }
     if( PORTBbits.RB2 !=0){  
         if (respondSearch!=0){
          chaine3= "message unicast";
          
          MiApp_WriteData(chaine1);
          MiApp_WriteData(chaine2);
          MiApp_WriteData(chaine3);
          MiApp_WriteData(chaine4);
          MiApp_WriteData(chaine6);
          MiApp_WriteData(chaine5);
          MiApp_WriteData(0);
          //MiApp_UnicastAddress(RX_ADRESS,false);
          
         }
         
     }
}

