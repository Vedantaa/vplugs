#define OSC_URI "http://lv2plug.in/plugins/oscillator"

/* includes */
#include "lv2/core/lv2.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <lv2/atom/atom.h>
#include <lv2/urid/urid.h>
#include <lv2/midi/midi.h>
#include <lv2/core/lv2_util.h>
#include <lv2/atom/util.h>





/* def synillator */
typedef struct
{
        float* output;
        float* level;
        const LV2_Atom_Sequence* midi_in;
        double rate;
        double pos;
        LV2_URID_Map*  map;
        struct {
                    LV2_URID midi_MidiEvent;
                  } urids;
        
       double freq;
       uint8_t note;
       uint8_t vel;
} syn;





/* LV2 methods */
// m is the syn struct that is operated on in the program. initialising vales for m
static LV2_Handle instantiate (const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
        syn* m = (syn*)calloc(1, sizeof(syn));
        m->rate = sample_rate;
        m->midi_in = NULL;
        m->pos = 0.0;
        m->urids.midi_MidiEvent =
                m->map->map(m->map->handle, LV2_MIDI__MidiEvent);
        

        return m;
}


// note: for ports, 0-midi, 1-level, 2-out, enumerate when more ports 
static void connect_port (LV2_Handle instance, uint32_t port, void *data)
{
        syn* m = (syn*)instance;
        if(!m) return;
        
        switch(port) 
        {
        case 0:
                m->midi_in = (LV2_Atom_Sequence*)data;
                break;

        case 1:  
                m->level = (float*)data;
                break;

        case 2: 
                m->output = (float*)data;
                break;

        default: 
                break;

        }

       }
static void activate (LV2_Handle instance)
{
//not needed
}


//create functions for midi handling here
void press(const uint8_t note, const uint8_t vel, syn* m)
{
     m->note = note;
     m->vel = vel;
     m->freq = (pow (2.0, ((double)(note) - 69.0) / 12.0) * 440.0);
}

void release(const uint8_t note, const uint8_t vel, syn* m)
{
        
     m->note = note;
     m->vel = vel;
}


static void run (LV2_Handle instance, uint32_t sample_count)
{
        syn* m = (syn*)instance;
        if (!m) return;
        /*check that all "connected ports" are not null pointers*/
        if ((!m->midi_in) || (!m->level) || (!m->output)) return;
        //midi handling 
        LV2_ATOM_SEQUENCE_FOREACH (m->midi_in, ev)
        {
        const uint32_t frame = ev->time.frames;
              // if(ev->body.type == m->urids.midi_MidiEvent) // check for midi-likness. URIDS used here
                {
                       const uint8_t* const msg = (const uint8_t*)(ev + 1); //check atom adress for midi info

                       switch (lv2_midi_message_type(msg)) // use info in atom
                        {
                                case LV2_MIDI_MSG_NOTE_ON:
                                        press(msg[1], msg[2], m);
                                        break;


                                case LV2_MIDI_MSG_NOTE_OFF:
                                        release(msg[1], msg[2], m); 
                                        break;
                        }

                }


        }
       for (uint32_t i = 0; i < sample_count; ++i)
         {
          m->output[i] =  sin (2.0 * M_PI * m->pos) * (m->vel / 127.0f); *m->level;
          m->pos += m->freq / m->rate;
         }

        /* commented out, but acts as a reference
         
         
        for(uint32_t i = 0; i<sample_count; i++)
        {
               m->output[i] = sin(2.0 * M_PI * m->pos) * *m->level;
               m->pos += *m->freq / m->rate;
        }

         */

}
static void deactivate (LV2_Handle instance)
{
//Not needed
}
static void cleanup (LV2_Handle instance)
{
        free(instance);
}
static const void* extension_data (const char *uri)
{      
       return NULL; 
}      
static LV2_Descriptor descriptor = {"https://github.com/Vedantaa/vplugs/vsynth",
                                          instantiate,
                                          connect_port,
                                          activate,
                                          run,
                                          deactivate,
                                          cleanup,
                                          extension_data};


/* interface */
const LV2_SYMBOL_EXPORT LV2_Descriptor* 	lv2_descriptor (uint32_t index)
{
        if(index == 0) return &descriptor;
        else return NULL;
}
