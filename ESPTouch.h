#ifndef ESPTOUCH_H
#define ESPTOUCH_H

#include <Arduino.h>
#include <stdint.h>
#include <deque>
#include <numeric>

#define NO_PRESS    0
#define PRESS       1
#define LONG_PRESS  2

#define MIN_T_BETWEEN_READS 10
#define PRESS_INPUT_THRESHOLD 15
#define NO_PRESS_THRESHOLD 45
#define MIN_T_FOR_TOUCH 150
#define MIN_T_BETWEEN_PRESS 500
#define READ_QUEUE_SIZE 20
#define PRESS_DURATION_FOR_LONG_PRESS 1000


struct TouchRead {
  TouchRead(uint8_t readValue, uint32_t readTime):v(readValue), t(readTime){};
  uint8_t v;
  uint32_t t;
};

class ESPTouch {

  public:

    ESPTouch(uint8_t pin):_pin(pin){};

    uint8_t read(){
      uint32_t ct = millis();

      //If not MIN_T_BETWEEN_READS return and do nothing
      if(_read.size() &&  (_read.back().t + MIN_T_BETWEEN_READS < ct ) ) {
        return 0;
      }
      _lastPress = 0;
      //Add new read value to the queue
      uint8_t inputValue = touchRead(_pin);
      _read.push_back(TouchRead(inputValue, ct));
      //Remove values if there are more than 10
      while( _read.size() > READ_QUEUE_SIZE ) {
        _read.erase( _read.begin() );
      }
      //Average the last 10 reads
      uint16_t readAverage = 0;
      for(uint8_t i = 0; i < _read.size(); i++) {
        readAverage += _read[i].v;
      }
      readAverage /= _read.size();

      if( readAverage < PRESS_INPUT_THRESHOLD ) {
        // Serial.println("Threshold met");
        //The variable keeping track of the length of the press is 0 so set it
        if(!_touchStart) {
          // Serial.println("Touch start detected");
          _touchStart = ct;
        }
        //Button has been held down for more than 1000 seconds
        else {
          if(ct - _touchStart > PRESS_DURATION_FOR_LONG_PRESS) {
            _depressed = true;
            _touchStart = ct;
          }
        }
      }
      else if( readAverage > NO_PRESS_THRESHOLD ) {
        if(_touchStart && !_depressed) {
          if(ct - _touchStart > MIN_T_FOR_TOUCH) {
            // Serial.println("End of touch");
            _touchStart = 0;
            _lastPress = ct;
            return PRESS;
          }
        }
        _touchStart = 0;
        _depressed = false;
      }

      return 0;
      
    }


  private:

    uint8_t _pin;
    uint8_t _low, _high;
    std::deque<TouchRead> _read;
    uint32_t _touchStart = 0, _lastPress = 0;
    bool _depressed = false;

};


#endif