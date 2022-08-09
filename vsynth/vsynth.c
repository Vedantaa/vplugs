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
                  } uris;

} syn;





/* LV2 methods */
// m is the syn struct that is operated on in the program. initialising vales for m
static LV2_Handle instantiate (const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
        syn* m = (syn*)calloc(1, sizeof(syn));
        m->rate = sample_rate;
        LV2_Atom_Sequence* m->midi_in = NULL;
        m->pos = 0.0;
        m->uris.midi_MidiEvent =
                m->map->map(self->map->handle, LV2_MIDI__MidiEvent);
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


static void run (LV2_Handle instance, uint32_t sample_count)
{
        syn* m = (syn*)instance;
        if (!m) return;
        /*check that all "connected ports" are not null pointers*/
        if ((!m->midi_in) || (!m->level) || (!m->output)) return;

        /*generation of sound with midi */
        for(uint32_t i = 0; i<sample_count; i++)
        {
               m->output[i] = sin(2.0 * M_PI * m->pos) * *m->level;
               m->pos += *m->freq / m->rate;
        }

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
