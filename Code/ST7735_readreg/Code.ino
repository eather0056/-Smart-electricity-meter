

#include<math.h>
#include <Adafruit_GFX.h>
 #include <SimpleTimer.h>
#include <Wire.h>
#include <SPI.h> 
#include <LiquidCrystal_I2C.h>
#include<SoftwareSerial.h>
#include <ArduinoJson.h>
#define SAMPLES 300   
#define ACS_Pin A0  


          #include "Adafruit_GFX.h"
          #include "Adafruit_ILI9341.h"
          
          #define TFT_DC 9              
          #define TFT_CS 10             
          #define TFT_RST 8                       
          #define TFT_CLK 13                    
          #define TFT_MISO 50          
          #define TFT_MOSI 51          
          
          
          Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);




        int decimalPrecision = 2;                   // decimal places for all values shown in LED Display & Serial Monitor


        /* 1 - Phase Angle, Frequency and Power Factor measurement */

        int expectedFrequency = 50;                 // Key in frequency for main grid (50 / 60 hz)
        int analogInputPin1PA = A3;                 // The input pin for analogRead1 sensor. Use voltage sensor as primary reference here.
        int analogInputPin2PA = A0;                 // The input pin for analogRead2 sensor. Use current or voltage as secondary reference.
        float voltageAnalogOffset =4;               // This is to offset analog value for analogInput1
        float currentAnalogOffset =-12;             // This is to offset analog value for analogInput2
        unsigned long startMicrosPA;                /* start counting time for Phase Angle and Period (in micro seconds)*/
        unsigned long vCurrentMicrosPA;             /* current time for analogInput1 (voltage) (in micro seconds). AnalogInput1 is used for reference for phase angle*/
        unsigned long iCurrentMicrosPA;             /* current time for analogInput2 (current/voltage) (in micro seconds).*/
        unsigned long periodMicrosPA;               /* current time for record period of wave */
        float vAnalogValue =0;                      /* is the analog value for voltage sensor / analogInput1 and center at 0 value */
        float iAnalogValue =0;                      /* is the analog value for current sensor / analogInput2 and center at 0 value */
        float previousValueV =0;                    /* use to record peak value for voltage sensor*/
        float previousValueI =0;                    /* use to record peak value for current sensro*/
        float previousphaseAngleSample=0;           /* previous sample reading to replace false value less than 100 micro seconds*/
        float phaseAngleSample =0;                  /* is the time difference between 2 sensor values (in micro seconds) */
        float phaseAngleAccumulate =0;              /* is the accumulate time difference for accumulate samples*/
        float periodSample=0;                       /* is the time difference for a period of wave for a sample (in micro seconds)*/
        float periodSampleAccumulate = 0;           /* is the accumulate time difference for accumulate samples */
        float phaseDifference =0;                   /* is the averaged set of time difference of 2 sensors*/
        float phaseAngle =0;                        /* is the phase angle in degree (out of 360)*/
        float frequency = 0;                        /* is the frequency of the voltage sensor wave*/
        float voltagePhaseAngle=0;                  /* is the time recorded from begining to reach peak value for analogInput1 in micro seconds*/
        float currentPhaseAngle=0;                  /* is the time recorded from begining to reach peak value for analogInput2 in micro seconds*/
        float averagePeriod =0;                     /* is the average set of time recorded for a period of wave */
        int sampleCount = 0;                        /* to count how many set of samples */
        int a = 3;                                  /* use for switching operation*/
        float powerFactor;                          /* to calculate power factor */
        
int R1 = 48;

//****************Voltage*********************************

        //int decimalPrecision = 2;                   // decimal places for all values shown in LED Display & Serial Monitor
        int VoltageAnalogInputPin = A3;            
        float voltageSampleRead  = 0;               /* to read the value of a sample in analog including voltageOffset1 */
        float voltageLastSample  = 0;               /* to count time for each sample. Technically 1 milli second 1 sample is taken */
        float voltageSampleSum   = 0;               /* accumulation of sample readings */
        float voltageSampleCount = 0;               /* to count number of sample. */
        float voltageMean ;                         /* to calculate the average value from all samples, in analog values*/ 
        float RMSVoltageMean ;                      /* square roof of voltageMean without offset value, in analog value*/
        float adjustRMSVoltageMean;
        float FinalRMSVoltage;                      /* final voltage value with offset value*/
   
        float voltageOffset1 =0.00 ;         
        float voltageOffset2 = 0.00;  

//-----------------------------Current-------------------------------------
        float High_peak,Low_peak;        
        float Amps_Peak_Peak, Amps_RMS;

void setup() 
{

        
        
          Serial.begin(9600);
          Serial.println("ILI9341 Test!"); 
         
          tft.begin();

          pinMode(INPUT,R1);

  
}

void loop() 
{


        /* 1 - Phase Angle, Frequency and Power Factor measurement */

        digitalWrite(R1, LOW);
        
        vAnalogValue = analogRead(analogInputPin1PA)-512 + voltageAnalogOffset;       /* read analogInput1 with center adjusted*/
        iAnalogValue = analogRead(analogInputPin2PA)-512 + currentAnalogOffset;       /* read analogInput2 with center adjusted*/

        if((vAnalogValue>0) && a == 3)                                          /* initial begining stage of measurement when analogInput1 wave larger than 0 */
        {
          a=0;                                                                  /* allow to change to the next stage */
        }
        
        if((vAnalogValue<=0) && a ==0)                                          /* when analog value of analogInput1 smaller or equal than 0*/
        {
          startMicrosPA = micros();                                             /* start counting time for all*/
          a=1;                                                                  /* allow to change to the next stage */
        }
        
        if((vAnalogValue>0) && a ==1)                                           /* when analog value of analogInput1 larger than 0*/
        {
          a = 2;                                                                /* allow to change to the next stage */
          previousValueV = 0;                                                   /* reset value. This value to be compared to measure peak value for analogInput1 */
          previousValueI = 0;                                                   /* reset value. This value to be compared to measure peak value for analogInput2 */
        }  
     
        if((vAnalogValue > previousValueV ) && a==2)                            /* if current measured value larger than previous peak value of analogInput1 */
        {
          previousValueV = vAnalogValue ;                                       /* record current measure value replace previous peak value */
          vCurrentMicrosPA = micros();                                          /* record current time for analogInput1 */
        }
        
        if((iAnalogValue > previousValueI) && a==2)                             /* if current measured value larger than previous peak value of analogInput2 */
        {
          previousValueI = iAnalogValue ;                                       /* record current measure value replace previous peak value */
          iCurrentMicrosPA = micros();                                          /* record current time for analogInput2 */
        }
  
        if((vAnalogValue <=0) && a==2)                                          /* when analog value of analogInput1 smaller or equal than 0*/
        {
          periodMicrosPA = micros();                                            /* record current time for 1 period */
          periodSample = periodMicrosPA - startMicrosPA;                        /* period wave is the current time minus the starting time (for 1 sample)*/
          periodSampleAccumulate = periodSampleAccumulate + periodSample;       /* accumulate or add up time for all sample readings of period wave */
          voltagePhaseAngle = vCurrentMicrosPA - startMicrosPA;                 /* time taken for analogInput1 from 0 (down wave) to peak value (up wave)*/
          currentPhaseAngle = iCurrentMicrosPA - startMicrosPA;                 /* time taken for analogInput2 from 0 (down wave) to peak value (up wave)*/
          phaseAngleSample = currentPhaseAngle - voltagePhaseAngle;             /* time difference between analogInput1 peak value and analogInput2 peak value*/
          if(phaseAngleSample>=100)                                             /* if time difference more than 100 micro seconds*/
          {
          previousphaseAngleSample = phaseAngleSample;                          /* replace previous value using new current value */ 
          }
          if(phaseAngleSample<100)                                              /* if time difference less than 100 micro seconds (might be noise or fake values)*/
          {
          phaseAngleSample = previousphaseAngleSample;                          /* take previous value instead using low value*/
          }
          phaseAngleAccumulate = phaseAngleAccumulate + phaseAngleSample;       /* accumulate or add up time for all sample readings of time difference */
          sampleCount = sampleCount + 1;                                        /* count sample number */
          startMicrosPA = periodMicrosPA;                                       /* reset begining time */
          a=1;                                                                  /* reset stage mode */
          previousValueV = 0;                                                   /* reset peak value for analogInput1 for next set */
          previousValueI = 0;                                                   /* reset peak value for analogInput2 for next set */
        }

        if(sampleCount == expectedFrequency)                                          /* if number of total sample recorded equal 50 by default */
        {
          averagePeriod = periodSampleAccumulate/sampleCount;                         /* average time for a period of wave from all the sample readings*/
          frequency = 1000000 / averagePeriod;                                        /* the calculated frequency value */
          phaseDifference = phaseAngleAccumulate / sampleCount;                       /* average time difference between 2 sensor peak values from all the sample readings */
          phaseAngle = ((phaseDifference*360) / averagePeriod);                       /* the calculated phase angle in degree (out of 360)*/
          powerFactor = cos(phaseAngle*0.017453292);                                  /* power factor. Cos is in radian, the formula on the left has converted the degree to rad. */
          Serial.print("Phase Angle :");
          Serial.print(phaseAngle,decimalPrecision);
          Serial.print("Â°  ");
          Serial.print("Frequency :");
          Serial.print(frequency,decimalPrecision);
          Serial.print("Hz  ");
          Serial.print("Power Factor :");
          Serial.println(powerFactor,decimalPrecision);

            tft.setCursor(8, 5);
            tft.setTextColor(ILI9341_RED);  tft.setTextSize(3);
            tft.println("Phase Angle :");
            tft.println(phaseAngle,decimalPrecision);

            delay(2000);
             tft.fillScreen(1);
             
          sampleCount = 0;                                                            /* reset the sample counting quantity */
          periodSampleAccumulate = 0;                                                 /* reset the accumulated time for period wave from all samples */
          phaseAngleAccumulate =0;                                                    /* reset the accumulated time for time difference from all samples*/
        
        
        yield();
        }
    read_Volt(); 
    read_Amps();                              
  Amps_RMS = 0.59*1000*Amps_Peak_Peak*0.3536*0.06;     //Now we have the peak to peak value normally the formula requires only multiplying times 0.3536                    
                                           
      if(Amps_RMS < 0.2)
         {Amps_RMS =0;}                                       
  Serial.println(Amps_RMS);  
       

}

void read_Amps()           
{                           
  int cnt;            
  High_peak = 0;      
  Low_peak = 1024;
  
      for(cnt=0 ; cnt<SAMPLES ; cnt++)       
      {
        float ACS_Value = analogRead(ACS_Pin); 

        
        if(ACS_Value > High_peak)              
            {
              High_peak = ACS_Value;           
            }
        
        if(ACS_Value < Low_peak)               
            {
              Low_peak = ACS_Value;            
            }
      }                                       
      
  Amps_Peak_Peak = High_peak - Low_peak;      
}


void read_Volt()           
{  
 // if(micros() >= voltageLastSample + 1000 )                                                                      /* every 0.2 milli second taking 1 reading */
     //     {
            voltageSampleRead = (analogRead(VoltageAnalogInputPin)- 512)+ voltageOffset1;                             /* read the sample value including offset value*/
            voltageSampleSum = voltageSampleSum + sq(voltageSampleRead) ;                                             /* accumulate total analog values for each sample readings*/
            
            voltageSampleCount = voltageSampleCount + 1;                                                              /* to move on to the next following count */
       //     voltageLastSample = micros() ;                                                                            /* to reset the time again so that next cycle can start again*/ 
       //   }
        
      //  if(voltageSampleCount == 1000)                                                                                /* after 4000 count or 800 milli seconds (0.8 second), do the calculation and display value*/
        //  {
            voltageMean = voltageSampleSum/voltageSampleCount;                                                        /* calculate average value of all sample readings taken*/
            RMSVoltageMean = (sqrt(voltageMean))*1.5;                                                                 // The value X 1.5 means the ratio towards the module amplification.      
            adjustRMSVoltageMean = RMSVoltageMean + voltageOffset2;                                                   /* square root of the average value including offset value */                                                                                                                                                       /* square root of the average value*/                                                                                                             
            FinalRMSVoltage = RMSVoltageMean + voltageOffset2;                                                        /* this is the final RMS voltage*/
            if(FinalRMSVoltage <= 10)                                                                                /* to eliminate any possible ghost value*/
            {FinalRMSVoltage = 0;}
//            lcd.setCursor ( 0, 0 ); 
//            lcd.print("RMS");
//            lcd.setCursor ( 0, 1); 
//            lcd.print("Voltages = ");
//            lcd.print(FinalRMSVoltage,decimalPrecision);
            Serial.println(FinalRMSVoltage,decimalPrecision);
            voltageSampleSum =0;                                                                                      /* to reset accumulate sample values for the next cycle */
            voltageSampleCount=0;                                                                                     /* to reset number of sample for the next cycle */
         // }
}
