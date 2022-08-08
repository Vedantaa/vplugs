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



/* def oscillator */
typedef struct
{
        float* output;
        float* level;
        float* freq;
        const LV2_Atom_Sequence* midi;
        double rate;
        double pos; 
} osc;


/* LV2 methods */
static LV2_Handle instantiate (const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
        osc* m = (osc*)calloc(1, sizeof(osc));
        m->rate = sample_rate;
        m->pos = 0.0;
        return m;
}
// note: for ports
static void connect_port (LV2_Handle instance, uint32_t port, void *data_location)
{
        osc* m = (osc*)instance;
        if(!m) return;
        
        switch(port) 
        {
        case 0:
                m->output = (float*)data_location;
                break;

        case 1:  
                m->freq = (float*)data_location;
                break;

        case 2: 
                m->level = (float*)data_location;
                break;

        default: 
                break;

        }

}
static void activate (LV2_Handle instance)
{

}


static void run (LV2_Handle instance, uint32_t sample_count)
{
        osc* m = (osc*)instance;
        if (!m) return;
        /*check that all "connected ports" are not null pointers*/
        if ((!m->output) || (!m->level) || (!m->freq)) return;

        /*generation of a sine sound*/
        for(uint32_t i = 0; i<sample_count; i++)
        {
               m->output[i] = sin(2.0 * M_PI * m->pos) * *m->level;
               m->pos += *m->freq / m->rate;
        }

}
static void deactivate (LV2_Handle instance)
{

}
static void cleanup (LV2_Handle instance)
{
        free(instance);
}
static const void* extension_data (const char *uri)
{      
       return NULL; 
}      
static LV2_Descriptor descriptor = {"https://github.com/Vedantaa/messrepo/osctest",
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
