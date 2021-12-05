# ESP32_HC-SRF04
Generic example on how to program a range finder sensor using interrupts.
The trigger signal is delivered through a hw_timer interrupt. 
The duration of the echo signal is measured via an external interrupt on the echo signal pin.

The onTimer() function is called every 50 uS and creates a trigger pulse every TICK_TOTAL calls (4000) corresponding to 200 mS. 
The pulse generator is implemented with a simple finite state machine contained in the switch() instruction. 
Each time this function is called, exactly one of the cases is executed. When the count reaches 200 mS, the state is changed from 0 to 1, 
which immediately causes the trigger pin to go to high level and the state to change to 2. When the function is called next 50uS later, 
the case 2 is called, which resets the trigger pin level to low and the state reset to 0.

The echoPulse() function is called whenever the echo input signal changes state and generates an interrupt. 
The echo signal input is tested to determine whether this is the start or end of the echo pulse and the start and end times are recorded accordingly. 
If the input changed from high level to low level then this is the end of the pulse and duration is calculated.
