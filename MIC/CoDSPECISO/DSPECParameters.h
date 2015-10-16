 

#ifndef DSPECPARAMETERS_H
#define DSPECPARAMETERS_H

//DSPECParameters.h

#define DPLEN 64

typedef struct _dspec_parameters
{
	char active_devices					[DPLEN];
	char blre_automatic_enable			[DPLEN];
	char correction_flat				[DPLEN];
	char correction_flat_list			[DPLEN];
	char date							[DPLEN];
	char date_start						[DPLEN];
	char device							[DPLEN];
	char gain_adjustment				[DPLEN];
	char gain_channel					[DPLEN];
	char gain_coarse					[DPLEN];
	char gain_coarse_list				[DPLEN];
	char gain_conversion				[DPLEN];
	char gain_conversion_list			[DPLEN];
	char gain_fine						[DPLEN];
	char gain_fine_list					[DPLEN];
	char gain_polarity					[DPLEN];
	char gain_stabilization_enable		[DPLEN];
	char gain_width						[DPLEN];
	char gate							[DPLEN];
	char hv								[DPLEN];
	char hv_enable						[DPLEN];
	char hv_list						[DPLEN];
	char hv_overload					[DPLEN];
	char hv_polarity					[DPLEN];
	char hv_volts						[DPLEN];
	char live							[DPLEN];
	char live_preset					[DPLEN];
	char lld							[DPLEN];
	char network_address				[DPLEN];
	char network_id						[DPLEN];
	char pz								[DPLEN];
	char pz_automatic					[DPLEN];
	char pz_enable						[DPLEN];
	char shap_cusp						[DPLEN];
	char shap_cusp_list					[DPLEN];
	char shap_flat						[DPLEN];
	char shap_flat_list					[DPLEN];
	char shap_rise						[DPLEN];
	char shap_rise_list					[DPLEN];
	char shutdown						[DPLEN];
	char shutdown_list					[DPLEN];
	char shutdown_type_selected			[DPLEN];
	char shut_actual					[DPLEN];
	char stabilization_mode				[DPLEN];
	char time							[DPLEN];
	char time_start						[DPLEN];
	char true_time						[DPLEN];
	char uld							[DPLEN];
	char version						[DPLEN];
	char zdt_enable						[DPLEN];

} DSPEC_PARAMETERS;

#endif //DSPECPARAMETERS_H