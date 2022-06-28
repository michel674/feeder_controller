/*
      Teste de função em C para funcionamento do CI HX711
      
      Conversor AD para célula de carga


      Eng. Wagner Rambo
      Outubro de 2016
*/

// --- Mapeamento de Hardware ---
#define  ADDO  7    //Data Out
#define  ADSK  6    //SCK


// --- Protótipo das Funções Auxiliares ---
unsigned long ReadCount(); //conversão AD do HX711


// --- Variáveis Globais ---
unsigned long convert;


// --- Configurações Iniciais ---
void setup()
{
   pinMode(ADDO, INPUT_PULLUP);   //entrada para receber os dados
   pinMode(ADSK, OUTPUT);         //saída para SCK
   
   Serial.begin(9600);

} //end setup


// --- Loop Infinito ---
void loop()
{
  
  convert = ReadCount();
  
  Serial.println(convert);
  
  delay(2000);



} //end loop


// --- Funções ---
unsigned long ReadCount()
{
  unsigned long Count = 0;
  unsigned char i;
  
  digitalWrite(ADSK, LOW);
  
  while(digitalRead(ADDO));
  
  for(i=0;i<24;i++)
  {
     digitalWrite(ADSK, HIGH);
     Count = Count << 1;
     digitalWrite(ADSK, LOW);
     if(digitalRead(ADDO)) Count++;
  
  } //end for
  
  digitalWrite(ADSK, HIGH);
  Count = Count^0x800000;
  digitalWrite(ADSK, LOW);
  
  return(Count);


} //end ReadCount









