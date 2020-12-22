#define PWM_PIN 9
#define DIR_PIN 4

#define BUF_SIZE  3

const byte adcPin = 0;
volatile int adcReading;
volatile boolean adcDone;
boolean adcStarted;
int value;

int buf[BUF_SIZE];
int buf_pos = 0;

float avg(int *a, int len) {
  float sum = 0;
  for (int i = 0; i < len; i++) {
    sum += a[i];
  }
  return sum / len;
}

int pwm_value = 0;

const float t_sin[] = {
-1.2246467991473532e-16, -0.03156854976481057, -0.0631056313126736,
-0.09457980779484526, -0.12595970506771778, -0.15721404296725092,
-0.18831166648971792, -0.2192215768476913, -0.24991296237030866,
-0.2803552292170146, -0.310518031874169, -0.3403713034041129,
-0.3698852854165468, -0.3990305577323413, -0.4277780677102099,
-0.4560991592070161, -0.4839656011428388, -0.5113496156423267,
-0.5382239057242889, -0.564561682511918, -0.5903366919365286,
-0.6155232409081793, -0.6400962229271072, -0.6640311431104312,
-0.6873041426091842, -0.709892022391333, -0.7317722663670769,
-0.7529230638333771, -0.7733233312153391, -0.7929527330827786,
-0.8117917024210209, -0.8298214601357259, -0.847024033772299,
-0.8633822754312235, -0.8788798788614604, -0.8935013957148742,
-0.9072322509454814, -0.9200587573381745, -0.9319681291524349,
-0.9429484948674371, -0.9529889090158393, -0.9620793630944628,
-0.9702107955409863, -0.9773751007667072, -0.9835651372363698,
-0.9887747345870026, -0.9929986997786696, -0.9962328222710067,
-0.9984738782203788, -0.9997196336934779, -0.9999688468941563,
-0.9992212694012756, -0.9974776464163387, -0.9947397160206569,
-0.9910102074427921, -0.9862928383380026, -0.9805923110824041,
-0.9739143080855378, -0.9662654861260218, -0.9576534697159296,
-0.9480868435005095, -0.9375751437008251, -0.9261288486078412,
-0.9137593681374366, -0.9004790324567516, -0.8863010796932087,
-0.8712396427384596, -0.8553097351604118, -0.8385272362373772,
-0.8209088751292625, -0.8024722142015779, -0.7832356315188904,
-0.7632183025251683, -0.7424401809292829, -0.7209219788147163,
-0.6986851459933064, -0.6757518486236087, -0.6521449471151867,
-0.6278879733408582, -0.6030051071796144, -0.5775211524135886,
-0.5514615120031074, -0.5248521627644684, -0.4977196294756829,
-0.47009095843600285, -0.441993690505579, -0.41345583365213406,
-0.38450583503201086, -0.3551725526334285, -0.3254852265102115,
-0.2954734496346699, -0.2651671383986786, -0.23459650279236877,
-0.20379201629015223, -0.1727843854740993, -0.14160451942495147,
-0.11028349891127495, -0.07885254540747648, -0.047342989971558086,
-0.0157862420136367, 0.015786242013637142, 0.047342989971558086,
0.07885254540747648, 0.11028349891127495, 0.1416045194249519,
0.17278438547409974, 0.20379201629015223, 0.23459650279236877,
0.2651671383986786, 0.2954734496346699, 0.32548522651021194,
0.3551725526334285, 0.38450583503201086, 0.41345583365213406,
0.441993690505579, 0.47009095843600324, 0.49771962947568327,
0.5248521627644684, 0.5514615120031074, 0.5775211524135886, 0.6030051071796144,
0.6278879733408586, 0.6521449471151867, 0.6757518486236087, 0.6986851459933064,
0.7209219788147163, 0.7424401809292832, 0.7632183025251683, 0.7832356315188904,
0.8024722142015782, 0.8209088751292625, 0.8385272362373775, 0.8553097351604118,
0.8712396427384596, 0.8863010796932088, 0.9004790324567516, 0.9137593681374369,
0.9261288486078411, 0.9375751437008251, 0.9480868435005096, 0.9576534697159296,
0.9662654861260219, 0.9739143080855377, 0.9805923110824041, 0.9862928383380027,
0.9910102074427921, 0.994739716020657, 0.9974776464163387, 0.9992212694012756,
0.9999688468941563, 0.9997196336934779, 0.9984738782203788, 0.9962328222710067,
0.9929986997786696, 0.9887747345870026, 0.9835651372363698, 0.9773751007667072,
0.9702107955409863, 0.9620793630944628, 0.9529889090158393, 0.942948494867437,
0.9319681291524347, 0.9200587573381745, 0.9072322509454813, 0.8935013957148742,
0.8788798788614604, 0.8633822754312231, 0.847024033772299, 0.8298214601357257,
0.8117917024210209, 0.7929527330827786, 0.7733233312153388, 0.7529230638333771,
0.7317722663670766, 0.709892022391333, 0.6873041426091842, 0.6640311431104315,
0.6400962229271072, 0.615523240908179, 0.5903366919365286, 0.564561682511918,
0.5382239057242889, 0.5113496156423267, 0.48396560114283843,
0.4560991592070161, 0.42777806771020943, 0.3990305577323413,
0.3698852854165468, 0.3403713034041125, 0.310518031874169, 0.2803552292170142,
0.24991296237030866, 0.2192215768476913, 0.18831166648971748,
0.15721404296725092, 0.12595970506771734, 0.09457980779484526,
0.0631056313126736, 0.031568549764810126, 1.2246467991473532e-16
};

void setup() {
//  TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz  
//  TCCR1B = TCCR1B & B11111000 | B00000100;    // set timer 1 divisor to   256 for PWM frequency of   122.55 Hz  
//  TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)
//  TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
  Serial.begin(115200);

  ADCSRA =  bit (ADEN);                     
  ADCSRA |= bit (ADPS2);
  ADMUX  =  bit (REFS0) | (adcPin & 0x07);

  pinMode(PWM_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
}

void loop() {
  if (adcDone) {
    adcStarted = false;
    value = adcReading;
    buf[buf_pos] = value;
    buf_pos = (buf_pos + 1) % BUF_SIZE;
    adcDone = false;
  }
    
  if (!adcStarted) {
    adcStarted = true;
    ADCSRA |= bit (ADSC) | bit (ADIE);
  }  

  float analogValue = 128.0 * t_sin[pwm_value];
  pwm_value = (pwm_value + 1) % 200;

  analogWrite(PWM_PIN, int(fabs(analogValue)));
  digitalWrite(DIR_PIN, analogValue > 0 ? HIGH : LOW);


  Serial.print(avg(buf, BUF_SIZE) - 512);
  Serial.print("\t");
  Serial.println(analogValue);
  delay(10);  
  
}

ISR (ADC_vect) {
  adcReading = ADC;
  adcDone = true;  
}
