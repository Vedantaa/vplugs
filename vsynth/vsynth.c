#define OSC_URI "http://lv2plug.in/plugins/oscillator"

/* includes */
#include "stddef.h"
#include "lv2/core/lv2.h"
#include <lv2/core/lv2_util.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <lv2/urid/urid.h>
#include <lv2/midi/midi.h>
#include "lv2/log/log.h"
#include "lv2/log/logger.h"





//
typedef struct 
{
	const LV2_Atom_Sequence* midi_in;
	LV2_URID_Map*  map;
	LV2_Log_Logger logger;
	struct {
			LV2_URID midi_MidiEvent;
			} urids;

	float* output;
	double rate;
	double pos;
	//state 1=on, 0=off for key press
	unsigned int state;
	
	//midi properties, could create own key structure.
	double freq;
	double vel;
} syn;




static LV2_Handle instantiate (const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
        syn* m = (syn*)calloc(1, sizeof(syn));
		
		const char* missing = lv2_features_query(
		features,
		LV2_LOG__log,  &m->logger.log, false,
		LV2_URID__map, &m->map,        true,
		NULL);
		// clang-format on

		lv2_log_logger_set_map(&m->logger, m->map);
		if (missing) {
		lv2_log_error(&m->logger, "Missing feature <%s>\n", missing);
		free(m);
		return NULL;
	}
		
		//map midi uris in syn structure m
        m->urids.midi_MidiEvent =
		m->map->map(m->map->handle, LV2_MIDI__MidiEvent);
		
		
		
		m->state = 0;
		m->vel = 0.0;
		m->rate = sample_rate;
		m->pos = 0.0;
        return (LV2_Handle)m;
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


//function for writing data into m->out.
static void write_out(syn* instance, uint32_t start, uint32_t end)
{
	for (uint32_t i = start; i < end; i++)
	{
		instance->output[i] = sin (2.0 * M_PI * instance->pos) * instance->vel;
        instance->pos += instance->freq/instance->rate;
	}
}

static void run (LV2_Handle instance, uint32_t sample_count)
{
	syn* m = (syn*)instance;
	
	if ((!m->midi_in) || (!m->output)) return;
	
	
	uint32_t last_frame = 0;
	LV2_ATOM_SEQUENCE_FOREACH (m->midi_in, ev)
	{
		
	
        
		//output frames until new event and update last_frame
        const uint32_t frame = ev->time.frames;
        write_out (m, last_frame, frame);
        last_frame = frame;
		
		
		
		
		//midi handling here
		if (ev->body.type == m->urids.midi_MidiEvent)
		{
			const uint8_t* const msg = (const uint8_t*) (ev + 1); //take midi message from event
            const uint8_t type = lv2_midi_message_type (msg);
			
			switch(type) 
			{
				case LV2_MIDI_MSG_NOTE_ON:
					//press key and take msg[1] note and msg[2] velocity
					m->state = 1;
					m->freq = pow (2.0, ((double)msg[1] - 69.0)/12) * 440.0;
					m->vel = msg[2];
					
					
					break;
				
				case LV2_MIDI_MSG_NOTE_OFF:
					//release pressed key
					
					if(m->state == 1) {
					m->state = 0;
					m->pos = 0;
					
					}
					break;
					
				case LV2_MIDI_MSG_CONTROLLER:
					
					if (msg[1] == LV2_MIDI_CTL_ALL_NOTES_OFF) m->state = 0;
					else if (msg[1] == LV2_MIDI_CTL_ALL_SOUNDS_OFF) m->state = 0;
					
					break;
				
				
			}

		}
	}
	
	write_out (m, last_frame, sample_count); //play last frames
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