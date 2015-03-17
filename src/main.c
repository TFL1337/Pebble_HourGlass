/*

Changelog 


*/

#include "pebble.h"
  
static bool initphase = true;

static bool ja = true;
static bool nein = false;
  
static bool *test[36][18]; 
static bool *test2[36][18];

static bool *test3[36][18];
static bool *test4[36][18];
static bool *test5[36][18];

static bool *test6[9];
static bool *test7[9];

static int sumlinks = 0;
static int sumrechts = 0;
static int flow = 0;

bool sim_in = false;
bool sim_out = false;

static Window *window;

//static bool showBatteryLoading = false;
//static short int bat = 0;
//static int batAnimation = 0;

static short int hour;
static short int minute;
static short int second;

static GColor fColor;
static GColor bColor;

static Layer *second_layer;

static int sim = 0;


static bool isNight() {
  bool renm = false;
  time_t akt = time(NULL);
  struct tm akt_time = *localtime(&akt);
  if ((akt_time.tm_hour >= 21) || (akt_time.tm_hour < 8))
  {
    renm = true;  
  }  
  return renm;  
}

static void set_night_mode()
{
  if (isNight())
  {
    fColor = GColorBlack;
    bColor = GColorWhite;    
  }
  else
  {
    fColor = GColorWhite;
    bColor = GColorBlack;    
  }  
}


static void pixelverdopplung(GContext* ctx,GPoint point, int verschx, int verschiebung){
  short int x = point.x;
  short int y = point.y;  
  //graphics_draw_pixel(ctx,GPoint(x+verschx,y+verschiebung));
  graphics_draw_pixel(ctx,GPoint(x*4+verschx,y*4+verschiebung));
  graphics_draw_pixel(ctx,GPoint(x*4+1+verschx,y*4+1+verschiebung));
  graphics_draw_pixel(ctx,GPoint(x*4+verschx,y*4+1+verschiebung)); 
  graphics_draw_pixel(ctx,GPoint(x*4+1+verschx,y*4+verschiebung));
}

static void berechne_tick(){
  short int moeglichkeiten = 0;
  //array simulieren
  /**if ((*test[18][0] == nein) && (*test[18][1] == nein)){
    test[18][0] = &ja;
  }**/
  
  if (sim_out == true)
    {test[18][0] = &ja;
    sim_out = false;}
  
  for (short int i = 1; i < 35; i++)    {
    for (short int  j = 0 ; j < 17; j++){
      
      
      if (*test[i][j] == ja)
          {      
          //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d : %d", i, j);
          if (*test[i][j+1] == nein)  //nachfolgefeld ist leer
            {
              test2[i][j] = &nein;  //aktuelles feld leeren
              test2[i][j+1] = &ja; //nachfolgefeld füllen
              //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d : %d gewandert", i, j+1);
            }
           else {  // nachfolgefeld ist nicht leer
             moeglichkeiten = 0;
             //prüfe alternativen
             if (*test[i-1][j+1] == nein){ //links unten ist leer
              
               moeglichkeiten = 1;
              }
              if (*test[i+1][j+1] == nein) //rechts unten ist leer
              {
               //test2[i][j] = &nein;
               //test2[i+1][j+1] = &ja;
               moeglichkeiten = moeglichkeiten + 2;
              }
              
             
             
              switch (moeglichkeiten){
                case 1: //NUR links unten ist leer
                  test2[i][j] = &nein;
                  test2[i-1][j+1] = &ja;
                break;
                case 2: //NUR rechts unten ist leer
                  test2[i][j] = &nein;
                  test2[i+1][j+1] = &ja;
                break;
                case 3: //es muss eine zufällige Auswahl getroffen werden
                  if (rand()%2+1 == 1) { //links
                    test2[i][j] = &nein;
                    test2[i-1][j+1] = &ja;
                  } else {
                    test2[i][j] = &nein; //rechts
                    test2[i+1][j+1] = &ja;                    
                  }                    
                break;
                default:
                break;                
              } 
           
           }
          }
    }
  }
   
  //array kopieren
  for (short int a = 0; a < 36; a++)    {
    for (short int b = 0 ; b < 18; b++){
    
      if (*test2[a][b] == ja){
        test[a][b] = &ja;
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "uebergebe: %d : %d", a, b);
      }
      else{
        test[a][b] = &nein;
      }
    }
  } 
  
  
}


static void berechne_tick2(){

  //array simulieren

  
  
  
  //int i = 18;
  for (short int j = 0; j < 17; j++){
    if ((*test3[18][j+1] == nein) && (*test3[18][j] == ja)) {
      test4[18][j+1] = &ja;
      test4[18][j] = &nein;
    }
  }
  
  if (*test3[18][17] == ja)
    {test4[18][17] = &nein;}
  
  
  

  
  //array kopieren
  for (short int a = 0; a < 36; a++)    {
    for (short int b = 0 ; b < 18; b++){
      
        if (*test4[a][b] == nein){
          test3[a][b] = &nein;
        }
        else{
          test3[a][b] = &ja;
        }
      
    }
  }
  
  bool links = true;
  short int zuf = rand()%2+1;
  if (zuf == 1)
  {links = true;}
  else {links = false;}
  
  if (sumlinks == 0) 
    {links = false;}
  
  if (sumrechts == 0)
    {links = true;}
  
  sumlinks = 0;
  sumrechts = 0;
  
  for (short int b = 0; b < 17; b++)    {
    for (short int a = 0 ; a < 18; a++){
      
      if (*test3[a][b] == ja){
        sumlinks++;
        if ((*test3[a+1][b] == nein) && (*test3[a+1][b+1] == nein) && (*test5[a+1][b+1] == ja) && (*test5[a+1][b] == ja)){
          if (((a == 17) && (links == true)) || (a<17)){
          test4[a][b] = &nein;
            test4[a+1][b+1] = & ja;}
          
        }
        else{
          //test3[a][b] = &ja;
        }
      }
    }
  }
  
  for (short int b = 0; b < 17; b++)    {
    for (short int a = 19 ; a < 36; a++){
      if (*test3[a][b] == ja){
        sumrechts++;
        if ((*test3[a-1][b] == nein) && (*test3[a-1][b+1] == nein) && (*test5[a-1][b+1] == ja) && (*test5[a-1][b] == ja)){
          if (((a == 19) && (links == false)) || (a>19)){
          
          test4[a][b] = &nein;
          test4[a-1][b+1] = & ja;}
          
        }
        else{
          //test3[a][b] = &ja;
        }
      }
    }
  }
  
  
  //array kopieren
  for (short int a = 0; a < 36; a++)    {
    for (short int b = 0 ; b < 18; b++){
      
        if (*test4[a][b] == nein){
          test3[a][b] = &nein;
        }
        else{
          test3[a][b] = &ja;
        }
      
    }
  }
  
  //übergabe
  if (*test3[18][17] == ja)
    {sim_in = true;}
  else {sim_in = false;}
  
  //test3[18][17] = & nein;
 
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "zeichne: %d", zz);
}

static void draw_top_sand(GContext*ctx, int timer){
  int x = 0;
  int y = 0;
  
  berechne_tick2();
    
  //array ausgeben
  for (int c = 0; c < 36; c++)
    {
    for (int d = 0 ; d < 18; d++){
      if (*test3[c][d] == ja)
          {
          x= c;
          y= d;        
          pixelverdopplung(ctx,GPoint(x,y),0,0); 
          }      
    }    
  }  
}

static void init_top_sand(){
  int z = 2;
  for (int i = 0; i < 17; i++)    {
    for (int j = 0 ; j < 19; j++){
      if (j+z < 19){
      test3[j+z][i] = &ja;
      test3[36-j-z][i] = &ja;  
      test4[j+z][i] = &ja;
      test4[36-j-z][i] = &ja;  
      test5[j+z][i] = &ja;
      test5[36-j-z][i] = &ja;  
      }
    }
    z++;
  }  
}

static void draw_bottom_sand(GContext* ctx, int timer){
  short int x = 0;
  short int y = 0;

  berechne_tick(); 
  
  
  
  
  //array ausgeben
  for (short int c = 0; c < 36; c++)
    {
    for (short int d = 0 ; d < 18; d++){
      if (*test[c][d] == ja)
          {
          x= c;
          y= d;
          
          //graphics_draw_pixel(ctx,GPoint(x, y));
        
          pixelverdopplung(ctx,GPoint(x,y),0,98);
        
          //APP_LOG(APP_LOG_LEVEL_DEBUG, "zeichne: %d : %d", i, j);
    
          }
      
    }
    
  }
 
   //APP_LOG(APP_LOG_LEVEL_DEBUG, "---------------------- %d ---------------------", sim);     
 
  
  //graphics_draw_pixel(ctx,GPoint(x, y));
}



static void draw_digit(GContext* ctx, int digit, int ver_x, int ver_y){
  switch (digit){
    case 0:
      pixelverdopplung(ctx,GPoint(3+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(2+ver_x,2+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(4+ver_x,4+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,5+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,5+ver_y),0,0);        
    break;
    case 1:
      pixelverdopplung(ctx,GPoint(3+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(4+ver_x,2+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(2+ver_x,4+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,5+ver_y),0,0);
    break;
    case 2:
      pixelverdopplung(ctx,GPoint(4+ver_x,0+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(0+ver_x,4+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,5+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(2+ver_x,6+ver_y),0,0);      
    break;
    case 3:
      pixelverdopplung(ctx,GPoint(4+ver_x,0+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,1+ver_y),0,0);
      //pixelverdopplung(ctx,GPoint(2+ver_x,2+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(0+ver_x,4+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,5+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,5+ver_y),0,0);         
    break;
    case 4:
      pixelverdopplung(ctx,GPoint(4+ver_x,0+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(2+ver_x,2+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(4+ver_x,2+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(2+ver_x,4+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(4+ver_x,4+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,5+ver_y),0,0);
    break;
    case 5:
      pixelverdopplung(ctx,GPoint(4+ver_x,0+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(2+ver_x,2+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(6+ver_x,2+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(0+ver_x,4+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,5+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,5+ver_y),0,0);   
    break;
    case 6:
      pixelverdopplung(ctx,GPoint(3+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(2+ver_x,2+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,5+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,5+ver_y),0,0);
    break;
    case 7:
      pixelverdopplung(ctx,GPoint(4+ver_x,0+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(6+ver_x,2+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(2+ver_x,4+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,5+ver_y),0,0);
    break;
    case 8:
      pixelverdopplung(ctx,GPoint(3+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,5+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,5+ver_y),0,0);
    break;    
    case 9:    
      pixelverdopplung(ctx,GPoint(3+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,1+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(5+ver_x,3+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(4+ver_x,4+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(1+ver_x,5+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(3+ver_x,5+ver_y),0,0);
    break;    
    default:
    break;   
  }  
}


static void draw_dp(GContext* ctx, int ver_x, int ver_y){
      pixelverdopplung(ctx,GPoint(4+ver_x,6+ver_y),0,0);
      pixelverdopplung(ctx,GPoint(6+ver_x,4+ver_y),0,0);

}

static void draw_time(GContext* ctx, int versch_x, int versch_y){
  
  draw_digit(ctx,hour/10,versch_x+0,versch_y+0);
  draw_digit(ctx,hour%10,versch_x+3,versch_y+3);
  if (second % 2 == 0)
    { draw_dp(ctx,versch_x+3,versch_y+3);}
 
  draw_digit(ctx,minute/10,versch_x+7,versch_y+7);
  draw_digit(ctx,minute%10,versch_x+10,versch_y+10);
}

static void berechne_z(){
  if (sim_in == true)
    {test6[0] = &ja;
    sim_in = false;}
  else {test6[0] = &nein;} 
  
  
  for (int a = 0; a < 8; a++)
    {
    if ((*test6[a+1] == nein) && (*test6[a] == ja)){
      test7[a+1] = &ja;
      test7[a] = &nein;
    }
  }
  
  if (*test6[8] == ja)
    {test7[8] = &nein;
    sim_out = true;}
  
  //array kopieren
  for (int b = 0; b <9; b++)
  {
    if (*test7[b] == ja)
    {
      test6[b] = &ja;
    }
    else{
      test6[b] = &nein;
    }
  }
  
}

static void draw_bridge_sand(GContext* ctx, int timer){
  
  berechne_z();  
  
  /**if (*test6[7] == ja)
    {test6[7] = &nein;
    sim_out = true;}**/
  flow = 0;
  //array ausgeben
  for (int j = 0; j <9; j++){
    if (*test6[j] == ja){
      pixelverdopplung(ctx,GPoint(18,j+16),0,0);
    flow++;}   
  }
}

static void init_arrays(){
  for (int i = 0; i < (int)(sizeof(test6) / sizeof(test6[0])); i++){
    test6[i] = &nein;
    test7[i] = &nein;
   // test6[i] = &ja;
   // test7[i] = &ja;
  }
  
  

  for (short int i = 0; i < (int)(sizeof(test) / sizeof(test[0])); i++){
    for (short int j = 0; j < (int)(sizeof(test[0]) / sizeof(test[0][0])); j++){
      test[i][j] = &nein;
      test2[i][j] = &nein;       
      test3[i][j] = &nein;
      test4[i][j] = &nein;
      test5[i][j] = &nein;
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "original: %d", test[i][j]); 
     
    }
    
  }
  
  
}

static void update_second_layer(Layer *layer, GContext* ctx) {
  
  graphics_context_set_stroke_color(ctx, fColor);
  graphics_context_set_fill_color(ctx, fColor);
  
  if (initphase == true)
    {
    draw_time(ctx,0,7);
    initphase = false;
  }
  else{ 
  draw_top_sand(ctx, sim);
  draw_bridge_sand(ctx, sim);
  draw_bottom_sand(ctx, sim);  
    draw_time(ctx,0,7);}
  
  if ((flow + sumlinks + sumrechts == 0) && (second == 0)){
    init_arrays();  
  init_top_sand();    
  }
  
}



static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
 //Einmal in der Minute
 if (tick_time -> tm_sec == 0)
 {
    minute = tick_time -> tm_min;
    hour = tick_time -> tm_hour; 
    set_night_mode();
    window_set_background_color(window, bColor);

 } 
 second = tick_time -> tm_sec;
 if (sim < 2520)
  { 
   sim++; 
   //sim = sim + 20;
  }
  else
  {
    sim = 0;
  }
  
  /**if (sim % 2 == 0)
  {
    test[35][0] = &ja;
  }  **/
  
 layer_mark_dirty(second_layer);
}  



//Batterie
/**static void battery_handler(BatteryChargeState charge_state) {
  if (charge_state.is_charging) {
    //snprintf(batteryStatus, sizeof(batteryStatus), "loading");
    showBatteryLoading = true;
    bat = charge_state.charge_percent;
  } else {
    //snprintf(batteryStatus, sizeof(batteryStatus), "%d%%", charge_state.charge_percent);
    bat = charge_state.charge_percent;
    showBatteryLoading = false;
  }
}**/




static void init(void) {
  setlocale(LC_TIME, "");
  srand(time(NULL));
  window = window_create();
  window_set_fullscreen(window, true);
    
  set_night_mode();
  window_set_background_color(window, bColor); 
  
  
  Layer *window_layer = window_get_root_layer(window);
  //GRect bounds = layer_get_bounds(window_layer);
  second_layer = layer_create(GRect(0, 0, 144, 168));
  //second_layer = layer_create(bounds);
  layer_set_update_proc(second_layer, update_second_layer);
  layer_add_child(window_layer, second_layer);

  
  //battery_state_service_subscribe(battery_handler);
  //battery_handler(battery_state_service_peek());

  
  time_t now = time(NULL);
  struct tm *start_time = localtime(&now);
  
  minute = start_time -> tm_min;
  hour = start_time -> tm_hour;
  second = start_time -> tm_sec;
  
   const bool animated = true;
  window_stack_push(window, animated);
  
  layer_mark_dirty(second_layer);
 
  
  init_arrays();  
  init_top_sand();
 
  
  
  for (short int z = 0; z < minute%10*60+second; z++){
    
    berechne_tick();
    berechne_z();
    berechne_tick2();
  
  }
      

  layer_mark_dirty(second_layer);
  
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick); 
  

}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  layer_destroy(second_layer);
  //battery_state_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}