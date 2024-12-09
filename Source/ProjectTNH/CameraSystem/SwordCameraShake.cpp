// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordCameraShake.h"

USwordCameraShake::USwordCameraShake()
{
	OscillationDuration = 0.5f;
	OscillationBlendInTime = 0.1f;
	OscillationBlendOutTime = 0.2f;

	RotOscillation.Pitch.Amplitude = 4.0f;
	RotOscillation.Pitch.Frequency = 4.0f;
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Pitch.Waveform = EOscillatorWaveform::PerlinNoise;

	LocOscillation.Y.Amplitude = 5.0f;
	LocOscillation.Y.Frequency = 1.0f;
	LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Y.Waveform = EOscillatorWaveform::SineWave;

	LocOscillation.Z.Amplitude = 5.0f;
	LocOscillation.Z.Frequency = 1.0f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Z.Waveform = EOscillatorWaveform::SineWave;
}
