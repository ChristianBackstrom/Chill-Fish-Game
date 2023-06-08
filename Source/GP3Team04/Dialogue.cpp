// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue.h"

bool UDialogue::TryGetDialogueData(int Index, USoundCue*& SoundCue, FString& Text)
{
	if (Index < DialogueDatas.Num())
	{
		Text = DialogueDatas[Index].Text;
		SoundCue = DialogueDatas[Index].SoundCue;
		return true;
	}

	return false;
}
