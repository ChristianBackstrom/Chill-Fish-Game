// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue.h"

bool UDialogue::TryGetDialogueData(int Index, USoundWave*& Sound, FString& Text)
{
	if (DialogueDatas.IsEmpty()) return false;

	Sound = nullptr;
	
	if (Index < DialogueDatas.Num())
	{
		Text = DialogueDatas[Index].Text;
		if (IsValid(DialogueDatas[Index].SoundCue))
			Sound = DialogueDatas[Index].SoundCue;
		return true;
	}

	return false;
}
