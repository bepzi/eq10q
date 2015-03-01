/***************************************************************************
 *   Copyright (C) 2011 by Pere Ràfols Soler                               *
 *   sapista2@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/***************************************************************************
This file is the implementation of the EQ plugin
This plugin is inside the Sapista Plugins Bundle
This file implements functionalities for a large numbers of equalizers
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>

#include "lv2.h"
#include "forge.h"
#include "util.h"
#include "urid.h"
#include "uris.h"

#include "gui/eq_defines.h"
#include "dsp/vu.h"
#include "dsp/db.h"
#include "dsp/filter.h"

//Data from CMake
#define NUM_BANDS @Eq_Bands_Count@
#define NUM_CHANNELS @Eq_Channels_Count@
#define EQ_URI @Eq_Uri@


typedef struct {
  //Plugin ports
  float *fBypass;
  float *fInGain;
  float *fOutGain;
  float *fBandGain[NUM_BANDS];
  float *fBandFreq[NUM_BANDS];
  float *fBandParam[NUM_BANDS];
  float *fBandType[NUM_BANDS];
  float *fBandEnabled[NUM_BANDS];
  float *fInput[NUM_CHANNELS];
  float *fOutput[NUM_CHANNELS];
  float *fVuIn[NUM_CHANNELS];
  float *fVuOut[NUM_CHANNELS];
  LV2_Atom_Sequence *notify_port;
  const LV2_Atom_Sequence* control_port;

  //Features
  LV2_URID_Map *map;
  
  //Forge for creating atoms
  LV2_Atom_Forge forge;
  LV2_Atom_Forge_Frame notify_frame;
  
  //Atom URID
  Eq10qURIs uris;
  double sampleRate;
        
  //Plugin DSP
  Filter *filter[NUM_BANDS];
  Buffers buf[NUM_BANDS][NUM_CHANNELS];
  Vu *InputVu[NUM_CHANNELS];
  Vu *OutputVu[NUM_CHANNELS];

  //FFT Analysis
  int fft_ix; //Index to follow buffers
  double *fft_in, *fft_out;
  fftw_plan fft_p;
  int fft_on;
  double fft_normalization;
} EQ;

static void cleanupEQ(LV2_Handle instance)
{
  EQ *plugin = (EQ *)instance;
  int i;
  for(i=0; i<NUM_BANDS; i++)
  {
    FilterClean(plugin->filter[i]);
  }

  for(i=0; i<NUM_CHANNELS; i++)
  {
    VuClean(plugin->InputVu[i]);
    VuClean(plugin->OutputVu[i]);
  }
  
  fftw_destroy_plan(plugin->fft_p);
  fftw_free(plugin->fft_in); fftw_free(plugin->fft_out);
  free(instance);
}

static void connectPortEQ(LV2_Handle instance, uint32_t port, void *data)
{
  EQ *plugin = (EQ *)instance;

  //Connect standar ports
  switch (port)
  {
    case EQ_BYPASS:
      plugin->fBypass = data;
    break;

    case EQ_INGAIN:
      plugin->fInGain = data;
    break;

    case EQ_OUTGAIN:
      plugin->fOutGain = data;
    break;

    default:
      //Connect audio input ports
      if(port >= PORT_OFFSET && port < (PORT_OFFSET + NUM_CHANNELS))
      {
        plugin->fInput[port - PORT_OFFSET] = data;
      }

      //Connect audio output ports
      if(port >= (PORT_OFFSET + NUM_CHANNELS) && port < (PORT_OFFSET + 2*NUM_CHANNELS))
      {
        plugin->fOutput[port - PORT_OFFSET - NUM_CHANNELS] = data;
      }

      //Connect BandGain ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + NUM_BANDS))
      {
        plugin->fBandGain[port - PORT_OFFSET - 2*NUM_CHANNELS] = data;
      }

      //Connect BandFreq ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + NUM_BANDS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 2*NUM_BANDS))
      {
        plugin->fBandFreq[port - PORT_OFFSET - 2*NUM_CHANNELS - NUM_BANDS] = data;
      }

      //Connect BandParam ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + 2*NUM_BANDS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 3*NUM_BANDS))
      {
        plugin->fBandParam[port - PORT_OFFSET - 2*NUM_CHANNELS - 2*NUM_BANDS] = data;
      }

      //Connect BandType ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + 3*NUM_BANDS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 4*NUM_BANDS))
      {
        plugin->fBandType[port - PORT_OFFSET - 2*NUM_CHANNELS - 3*NUM_BANDS] = data;
      }

      //Connect BandEnabled ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + 4*NUM_BANDS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS))
      {
        plugin->fBandEnabled[port - PORT_OFFSET - 2*NUM_CHANNELS - 4*NUM_BANDS] = data;
      }

      //Connect VuInput ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS + NUM_CHANNELS))
      {
        plugin->fVuIn[port - PORT_OFFSET - 2*NUM_CHANNELS - 5*NUM_BANDS] = data;
      }

      //Connect VuOutput ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS + NUM_CHANNELS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS + 2*NUM_CHANNELS))
      {
        plugin->fVuOut[port - PORT_OFFSET - 2*NUM_CHANNELS - 5*NUM_BANDS - NUM_CHANNELS] = data;
      }
      
      //Connect Atom notify_port output port to GUI
      else if(port == PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS + 2*NUM_CHANNELS)
      {
        plugin->notify_port = (LV2_Atom_Sequence*)data;
      }
      
      //Connect Atom control_port input port from GUI
      else
      {
        plugin->control_port = (const LV2_Atom_Sequence*)data;
      }
    break;
  }
}

static LV2_Handle instantiateEQ(const LV2_Descriptor *descriptor, double s_rate, const char *path, const LV2_Feature *const * features)
{
  int i,ch;
  EQ *plugin_data = (EQ *)malloc(sizeof(EQ));  
  plugin_data->sampleRate = s_rate;
  
  for(i=0; i<NUM_BANDS; i++)
  {
    plugin_data->filter[i] = FilterInit(s_rate);
    for(ch=0; ch<NUM_CHANNELS; ch++)
    {
      flushBuffers(&plugin_data->buf[i][ch]);
    }
  }
  

  for(ch=0; ch<NUM_CHANNELS; ch++)
  {
    plugin_data->InputVu[ch] = VuInit(s_rate);
    plugin_data->OutputVu[ch] = VuInit(s_rate);
  }
  
  
  // Get host features
  for (i = 0; features[i]; ++i) 
  {
    if (!strcmp(features[i]->URI, LV2_URID__map))
    {
        plugin_data->map = (LV2_URID_Map*)features[i]->data;
    } 
  }
  if (!plugin_data->map)
  {
    printf("EQ10Q Error: Host does not support urid:map\n");
    goto fail;
  }

  // Map URIs and initialise forge
  map_eq10q_uris(plugin_data->map, &plugin_data->uris);
  lv2_atom_forge_init(&plugin_data->forge, plugin_data->map);
  
  //Initialize FFT objects
  plugin_data->fft_ix = 0;
  plugin_data->fft_in = (double*) fftw_malloc(sizeof(double) * FFT_N);
  plugin_data->fft_out = (double*) fftw_malloc(sizeof(double) * FFT_N);
  plugin_data->fft_p = fftw_plan_r2r_1d(FFT_N, plugin_data->fft_in, plugin_data->fft_out, FFTW_R2HC, FFTW_ESTIMATE);
  plugin_data->fft_on = 0; //Initialy no GUI then no need to compute FFT
  plugin_data->fft_normalization = pow(2.0/ ((double) FFT_N), 2.0);
  
  return (LV2_Handle)plugin_data;
  
  fail:
    free(plugin_data);
    return 0;
}

static void runEQ_v2(LV2_Handle instance, uint32_t sample_count)
{
  
  EQ *plugin_data = (EQ *)instance;
 
  //Get values of control ports
  const int iBypass = *(plugin_data->fBypass) > 0.0f ? 1 : 0;  
  const float fInGain = dB2Lin(*(plugin_data->fInGain));
  const float fOutGain = dB2Lin(*(plugin_data->fOutGain));
  int bd, pos; //loop index
   

  //Set up forge to write directly to notify output port.
  const uint32_t notify_capacity = plugin_data->notify_port->atom.size;
  lv2_atom_forge_set_buffer(&plugin_data->forge, (uint8_t*)plugin_data->notify_port, notify_capacity);
  lv2_atom_forge_sequence_head(&plugin_data->forge, &plugin_data->notify_frame, 0);
  //printf("Notify port size %d\n", notify_capacity);
   
  //Interpolation coefs force to recompute
  int recalcCoefs[NUM_BANDS];
  int forceRecalcCoefs = 0;
  
  int fft_send = 0; //If 1 LV2::Atom must send fftbuffer
  double fftInSample; //Sample to push throught the FFT buffer
  double  sampleL; //Current processing sample left signal
  #if NUM_CHANNELS == 2
  double sampleR; //Current processing sample right signal
  #endif
   
  //Read EQ Ports and mark to recompute if changed
  for(bd = 0; bd<NUM_BANDS; bd++)
  {
    if(dB2Lin(*(plugin_data->fBandGain[bd])) != plugin_data->filter[bd]->gain ||
	*plugin_data->fBandFreq[bd] != plugin_data->filter[bd]->freq ||
	*plugin_data->fBandParam[bd] != plugin_data->filter[bd]->q ||
	(int)(*plugin_data->fBandType[bd]) != plugin_data->filter[bd]->iType ||
	*plugin_data->fBandEnabled[bd] != plugin_data->filter[bd]->enable)
    {
      recalcCoefs[bd] = 1;
      forceRecalcCoefs = 1;
    }
    else
    {
      recalcCoefs[bd] = 0;
    }
  }
  
  //Read input Atom control port (Data from GUI)
  if(plugin_data->control_port)
  {
    const LV2_Atom_Event* ev = lv2_atom_sequence_begin(&(plugin_data->control_port)->body);
    // For each incoming message...
    while (!lv2_atom_sequence_is_end(&plugin_data->control_port->body, plugin_data->control_port->atom.size, ev)) 
    {
      // If the event is an atom:Object
      if (ev->body.type == plugin_data->uris.atom_Object)
      {
        const LV2_Atom_Object* obj = (const LV2_Atom_Object*)&ev->body;
        if (obj->body.otype == plugin_data->uris.atom_fft_on)
        {
          plugin_data->fft_on = 1;
        }
        else if(obj->body.otype == plugin_data->uris.atom_fft_off)
        {
          plugin_data->fft_on = 0;
          plugin_data->fft_ix = 0;
        }
        //printf("FFT ON: %d\n", plugin_data->fft_on);
      }
      ev = lv2_atom_sequence_next(ev);
    }
  }
    
  //Compute the filter
  for (pos = 0; pos < sample_count; pos++) 
  {       
    //Get input
    sampleL = (double)plugin_data->fInput[0][pos];
    DENORMAL_TO_ZERO(sampleL);
    
    #if NUM_CHANNELS == 2
    sampleR = (double)plugin_data->fInput[1][pos];
    DENORMAL_TO_ZERO(sampleR);
    #endif    
    
    //The input amplifier
    sampleL *= fInGain;
    fftInSample = sampleL;
    //Update VU input sample
    SetSample(plugin_data->InputVu[0], sampleL);

    #if NUM_CHANNELS == 2
    //The input amplifier
    sampleR *= fInGain;    
    fftInSample = 0.5*sampleL + 0.5*sampleR;
    //Update VU input sample
    SetSample(plugin_data->InputVu[1], sampleR);
    #endif    

    //Process every band
    if(!iBypass)
    {           
      //FFT of input data after input gain
      if(plugin_data->fft_on)
      {
        //Hanning Windowing
        plugin_data->fft_in[plugin_data->fft_ix] = fftInSample* 0.5 * (1.0-cos((2.0*PI*((double)plugin_data->fft_ix))/((double)(FFT_N-1))));
        
        //Hamming Windowing
        //plugin_data->fft_in[plugin_data->fft_ix] = fftInSample * (0.54 - 0.46*cos((2.0*PI*((double)plugin_data->fft_ix))/((double)(FFT_N-1))));
        
        plugin_data->fft_ix++;     
        if(plugin_data->fft_ix == FFT_N)
        {
          //FFT inout buffer full compute
          fftw_execute(plugin_data->fft_p);
          
          //Compute FFT Normalized Magnitude^2 
          double real, img;
          int ffti;
          for(ffti = 0; ffti<= FFT_N/2; ffti++)
          {
            real = plugin_data->fft_out[ffti];
            if(ffti > 0 && ffti < (FFT_N/2))
            {
              img = plugin_data->fft_out[FFT_N -ffti];
            }
            else
            {
              img = 0.0;
            }
            plugin_data->fft_out[ffti] = plugin_data->fft_normalization*(real*real + img*img);
          }
          
          plugin_data->fft_ix = 0;      
          fft_send = 1;
        }
      }
      
      //Coefs Interpolation
      if(forceRecalcCoefs)
      {
	for(bd = 0; bd<NUM_BANDS; bd++)
	{
	  if(recalcCoefs[bd])
	  {
	    calcCoefs(plugin_data->filter[bd],
		    dB2Lin(*(plugin_data->fBandGain[bd])),
		    *plugin_data->fBandFreq[bd],
		    *plugin_data->fBandParam[bd],
		    (int)(*plugin_data->fBandType[bd]),
		    *plugin_data->fBandEnabled[bd]);
	    }
	  }
	}
      
      
      //EQ PROCESSOR
	computeFilter(plugin_data->filter[0], &plugin_data->buf[0][0],&sampleL);
	
	#if NUM_BANDS >= 4
	computeFilter(plugin_data->filter[1], &plugin_data->buf[1][0],&sampleL);
	computeFilter(plugin_data->filter[2], &plugin_data->buf[2][0],&sampleL);
	computeFilter(plugin_data->filter[3], &plugin_data->buf[3][0],&sampleL);
	#endif
	
	#if NUM_BANDS >= 6
	computeFilter(plugin_data->filter[4], &plugin_data->buf[4][0],&sampleL);
	computeFilter(plugin_data->filter[5], &plugin_data->buf[5][0],&sampleL);
	#endif
	
	#if NUM_BANDS ==10
	computeFilter(plugin_data->filter[6], &plugin_data->buf[6][0],&sampleL);
	computeFilter(plugin_data->filter[7], &plugin_data->buf[7][0],&sampleL);
	computeFilter(plugin_data->filter[8], &plugin_data->buf[8][0],&sampleL);
	computeFilter(plugin_data->filter[9], &plugin_data->buf[9][0],&sampleL);
	#endif
	
	#if NUM_CHANNELS == 2
	  computeFilter(plugin_data->filter[0], &plugin_data->buf[0][1],&sampleR);
	  
	  #if NUM_BANDS >= 4
	  computeFilter(plugin_data->filter[1], &plugin_data->buf[1][1],&sampleR);
	  computeFilter(plugin_data->filter[2], &plugin_data->buf[2][1],&sampleR);
	  computeFilter(plugin_data->filter[3], &plugin_data->buf[3][1],&sampleR);
	  #endif
	  
	  #if NUM_BANDS >= 6
	  computeFilter(plugin_data->filter[4], &plugin_data->buf[4][1],&sampleR);
	  computeFilter(plugin_data->filter[5], &plugin_data->buf[5][1],&sampleR);
	  #endif
	  
	  #if NUM_BANDS ==10
	  computeFilter(plugin_data->filter[6], &plugin_data->buf[6][1],&sampleR);
	  computeFilter(plugin_data->filter[7], &plugin_data->buf[7][1],&sampleR);
	  computeFilter(plugin_data->filter[8], &plugin_data->buf[8][1],&sampleR);
	  computeFilter(plugin_data->filter[9], &plugin_data->buf[9][1],&sampleR);
	  #endif
	
	#endif
           
      //The output amplifier
      sampleL *= fOutGain; 
      //Update VU output sample
      SetSample(plugin_data->OutputVu[0], sampleL);
      
      #if NUM_CHANNELS == 2
      //The output amplifier
      sampleR *= fOutGain;
      //Update VU output sample
      SetSample(plugin_data->OutputVu[1], sampleR);
      #endif
    }

    //Write on output
    plugin_data->fOutput[0][pos] = (float)sampleL;
    #if NUM_CHANNELS == 2
    plugin_data->fOutput[1][pos] = (float)sampleR;
    #endif
  }
  
  //Update VU ports
  *(plugin_data->fVuIn[0]) = ComputeVu(plugin_data->InputVu[0], sample_count);
  *(plugin_data->fVuOut[0]) = ComputeVu(plugin_data->OutputVu[0], sample_count);
  #if NUM_CHANNELS == 2
  *(plugin_data->fVuIn[1]) = ComputeVu(plugin_data->InputVu[1], sample_count);
  *(plugin_data->fVuOut[1]) = ComputeVu(plugin_data->OutputVu[1], sample_count);
  #endif
  
  //Send sample Rate and FFT data vector some day in the future...
  LV2_Atom_Forge_Frame frame;       
  lv2_atom_forge_frame_time(&plugin_data->forge, 0);
  lv2_atom_forge_object( &plugin_data->forge, &frame, 0, plugin_data->uris.Dsp_2_Ui_COM); 
  lv2_atom_forge_key(&plugin_data->forge, plugin_data->uris.atom_sample_rate);
  lv2_atom_forge_double(&plugin_data->forge, plugin_data->sampleRate); 
  //Send FFT frame
  lv2_atom_forge_key(&plugin_data->forge, plugin_data->uris.atom_fft_data);
  lv2_atom_forge_vector(&plugin_data->forge, sizeof(double), plugin_data->uris.atom_Double, fft_send * (FFT_N/2), plugin_data->fft_out);
  lv2_atom_forge_pop(&plugin_data->forge, &frame);
  
  // Close off sequence
  lv2_atom_forge_pop(&plugin_data->forge, &plugin_data->notify_frame);
      
}

static const LV2_Descriptor eqDescriptor = {
  EQ_URI,
  instantiateEQ,
  connectPortEQ,
  NULL,
  runEQ_v2,
  NULL,
  cleanupEQ,
  NULL
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &eqDescriptor;
  default:
    return NULL;
  }
}
