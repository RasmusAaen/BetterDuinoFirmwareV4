#include "MarcDuinoDome.h"
#include "PanelSequences.h"

MarcDuinoDome::MarcDuinoDome(VarSpeedServo& Servo1, VarSpeedServo& Servo2, VarSpeedServo& Servo3, VarSpeedServo& Servo4, VarSpeedServo& Servo5, 
    VarSpeedServo& Servo6, VarSpeedServo& Servo7, VarSpeedServo& Servo8, VarSpeedServo& Servo9, VarSpeedServo& Servo10, 
    VarSpeedServo& Servo11, VarSpeedServo& Servo12, VarSpeedServo& Servo13) :
    MarcDuinoBase(Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11, Servo12, Servo13),
    Sequencer(this)
{
     // Initilize Sound Bank Overview for Random Songs
    MaxSoundsPerBank[0] = 0;
    for (int i = 1; i <= MAX_SOUND_BANK; i++)
        MaxSoundsPerBank[i] = Storage.getMaxSound(i);    

    ServoBuzzMillis     = millis();
    ServoBuzzIntervall  = SERVO_BUZZ_MILLIS;    // TODO Make EEPROM setting    
  
}

void MarcDuinoDome::init()
{
    MarcDuinoBase::init();
    Sequencer.init();

    // AUX1 Port
    pinMode(P_AUX1, OUTPUT);
    digitalWrite(P_AUX1, LOW);    
}

void MarcDuinoDome::run()
{
    MarcDuinoBase::run();
    Sequencer.run();

    // AUX1
    if ((AUX1Duration != 0) && (AUX1Duration != 99))
    {
        if ((millis() - AUX1Millis) > AUX1Duration)
        {
            digitalWrite(P_AUX1, LOW);
            AUX1Duration = 0;
        }
    }    
}

void MarcDuinoDome::adjustHoloEndPositions(Holo* Holos[], const unsigned int MinHolo, const unsigned int MaxHolo)
{
    word MinHValue   = 0;
    word MaxHValue   = 0;
    word MinVValue   = 0;
    word MaxVValue   = 0;
    unsigned int Index  = 0;

    for (unsigned int i=MinHolo; i<= MaxHolo; i++)
    {
        if (Storage.getIndividualSettings() == 0x01)
            Index = i;
        else
            Index = 0;

        MinHValue = Storage.getHoloHMinPos(Index);
        MaxHValue = Storage.getHoloHMaxPos(Index);
        MinVValue = Storage.getHoloVMinPos(Index);
        MaxVValue = Storage.getHoloVMaxPos(Index);

        // Set Direction
        if ((Storage.getHoloHDirection(0) == 1) || (Storage.getHoloHDirection(i) == 1)) // Reverse Servo
            Holos[i]->setHorizontalEndPositions(MaxHValue, MinHValue);
        else    // Normal
            Holos[i]->setHorizontalEndPositions(MinHValue, MaxHValue);

        if ((Storage.getHoloVDirection(0) == 1) || (Storage.getHoloVDirection(i) == 1)) // Reverse Servo
            Holos[i]->setVerticalEndPositions(MaxVValue, MinVValue);
        else    // Normal
            Holos[i]->setVerticalEndPositions(MinVValue, MaxVValue);
    }
}

void MarcDuinoDome::adjustPanelEndPositions(Panel* Panels[], const unsigned int MinPanel, const unsigned int MaxPanel)
{
    word OpenPos        = 0;
    word ClosedPos      = 0;
    unsigned int Index  = 0;

    for (unsigned int i=MinPanel; i<= MaxPanel; i++)
    {
        if (Storage.getIndividualSettings() == 0x01)
            Index = i;
        else
            Index = 0;

        // Set Direction
        if ((Storage.getServoDirection(0) == 1) || (Storage.getServoDirection(i) == 1)) // Reverse Servo
        {
            OpenPos     = Storage.getServoClosedPos(Index);
            ClosedPos   = Storage.getServoOpenPos(Index);
        }
        else    // Normal
        {
            OpenPos     = Storage.getServoOpenPos(Index);
            ClosedPos   = Storage.getServoClosedPos(Index);
        }
        Panels[i]->setEndPositions(OpenPos, ClosedPos);
    }
}

bool MarcDuinoDome::separateSoundCommand(const char* command, char* cmd, unsigned int & bank, unsigned int & sound)
{
    bank = 0;
    sound = 0;

    //if ((strlen(command) != 4) && (strlen(command) != 2))
    if ((strlen(command) <2) || (strlen(command) >4))
    {
        Serial.printf(F("Invalid Size: %i\r\n"), strlen(command));
        return false;
    }
    
    if (strlen(command) == 2)
    {
        memcpy(cmd, command+1, 1);
    }
    else if (strlen(command) == 3)
    {
        char bank_char[2];
        char sound_char[3];

        memset(cmd, 0x00, 1);

        memset(bank_char, 0x00, 2);
        memset(sound_char, 0x00, 3);

        memcpy(bank_char, command+1, 1);
        memcpy(sound_char, command+2, 1);

        bank=atoi(bank_char);
        sound=atoi(sound_char);
    }
    else if (strlen(command) == 4)
    {
        char bank_char[2];
        char sound_char[3];

        memset(cmd, 0x00, 1);

        memset(bank_char, 0x00, 2);
        memset(sound_char, 0x00, 3);

        memcpy(bank_char, command+1, 1);
        memcpy(sound_char, command+2, 2);

        bank=atoi(bank_char);
        sound=atoi(sound_char);
    }
    return true;
}

void MarcDuinoDome::getRandomSound(unsigned int & bank, unsigned int & sound)
{
    bank = random(1,6);
    sound = random(1, MaxSoundsPerBank[bank]+1);
}

void MarcDuinoDome::AUX1(const unsigned int Duration)
{
    switch (Duration)
    {
    case 0:
        digitalWrite(P_AUX1, LOW);
        AUX1Duration = 0;
        break;
    case 99:
        digitalWrite(P_AUX1, HIGH);
        AUX1Duration = 0;
        break;
    default:
        AUX1Millis = millis();
        AUX1Duration = Duration*1000;
        digitalWrite(P_AUX1, HIGH);
        break;
    }
}

void MarcDuinoDome::playSequence(const unsigned int SeqNr)
{
    Sequencer.stopSequence();
    Sequencer.clearSequence();
    
    switch (SeqNr)
    {
    case 0: // CLOSE ALL PANELS
        Sequencer.loadSequence(panel_init, SEQ_SIZE(panel_init));
        Sequencer.setServoSpeed(MarcDuinoSequencer::slow);
        break;
    case 1:  // SCREAM
    case 51: // Panel only Version
        Sequencer.loadSequence(panel_all_open, SEQ_SIZE(panel_all_open));
        Sequencer.setServoSpeed(MarcDuinoSequencer::slow);
        break;
    case 2: // WAVE
    case 52:// Panel only Version
        Sequencer.loadSequence(panel_wave, SEQ_SIZE(panel_wave));
        Sequencer.setServoSpeed(MarcDuinoSequencer::full);
        break;
    case 3: // MOODY FAST WAVE
    case 53:// Panel only Version
        Sequencer.loadSequence(panel_fast_wave, SEQ_SIZE(panel_fast_wave));
        Sequencer.setServoSpeed(MarcDuinoSequencer::full);
        break;
    case 4: // OPEN WAVE
    case 54:// Panel only Version
        Sequencer.loadSequence(panel_open_close_wave, SEQ_SIZE(panel_open_close_wave));
        Sequencer.setServoSpeed(MarcDuinoSequencer::full);
        break;
    case 5: // Beep Cantina (R2 beeping the cantina, panels doing marching ants)
    case 55:// Panel only Version
        Sequencer.loadSequence(panel_marching_ants, SEQ_SIZE(panel_marching_ants));
        Sequencer.setServoSpeed(MarcDuinoSequencer::slow);
        break;
    case 6: // SHORT CIRCUIT / FAINT
    case 56:// Panel only Version
        Sequencer.loadSequence(panel_all_open_long, SEQ_SIZE(panel_all_open_long));
        Sequencer.setServoSpeed(MarcDuinoSequencer::super_slow);
        break;
    case 7: // Cantina (Orchestral Cantina, Rhythmic Panels)
    case 57:// Panel only Version
        Sequencer.loadSequence(panel_dance, SEQ_SIZE(panel_dance));
        Sequencer.setServoSpeed(MarcDuinoSequencer::full);
        break;
    case 8: // LEIA
        Sequencer.loadSequence(panel_init, SEQ_SIZE(panel_init));	// Close panels
        Sequencer.setServoSpeed(MarcDuinoSequencer::slow);
        break;
    case 9:	// DISCO
        Sequencer.loadSequence(panel_long_disco, SEQ_SIZE(panel_long_disco)); // 6:26 seconds sequence
        Sequencer.setServoSpeed(MarcDuinoSequencer::full);
        break;
    case 10: // QUIET   sounds off, holo stop, panel closed
        Sequencer.loadSequence(panel_init, SEQ_SIZE(panel_init));
        Sequencer.setServoSpeed(MarcDuinoSequencer::slow);
        // stop_command(0);					// all panels off RC        
        break;
    case 11: // WIDE AWAKE	random sounds, holos on random, panels closed
        Sequencer.loadSequence(panel_init, SEQ_SIZE(panel_init));
        Sequencer.setServoSpeed(MarcDuinoSequencer::full);
        //stop_command(0);					// all panels off RC and closed
        break;
    case 12: // TOP PIE PANELS RC
        /*
        rc_command(7);
        rc_command(8);
        rc_command(9);
        rc_command(10);
        */
        break;
    case 13: // AWAKE	random sounds, holos off, panels closed
        Sequencer.loadSequence(panel_init, SEQ_SIZE(panel_init));
        Sequencer.setServoSpeed(MarcDuinoSequencer::slow);
        // stop_command(0);					// all panels off RC and closed
        break;
    case 14: // EXCITED	random sounds, holos movement, holo lights on, panels closed
        Sequencer.loadSequence(panel_init, SEQ_SIZE(panel_init));
        Sequencer.setServoSpeed(MarcDuinoSequencer::slow);
        //stop_command(0);					// all panels off RC and closed
        break;

    case 15: // SCREAM no panels: sound + lights but no panels
        break;
    case 16: // Panel Wiggle
        Sequencer.loadSequence(panel_wiggle, SEQ_SIZE(panel_wiggle));
        Sequencer.setServoSpeed(MarcDuinoSequencer::medium);
        break;

    case 58: // Panel Wave Bye Bye
        Sequencer.loadSequence(panel_bye_bye_wave, SEQ_SIZE(panel_bye_bye_wave));
        Sequencer.setServoSpeed(MarcDuinoSequencer::slow);
        break;

    default:
        break;         
    }
    playSequenceAddons(SeqNr);
}
