// Fill out your copyright notice in the Description page of Project Settings.


#include "STargetDummy.h"

#include "SAttributeComponent.h"

// Sets default values
ASTargetDummy::ASTargetDummy()
{
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>("meshComp");
	RootComponent = meshComp;

	attributeComp = CreateDefaultSubobject<USAttributeComponent>("attributeComp");
	attributeComp->OnHealthChanged.AddDynamic(this, &ASTargetDummy::OnHealthChanged);

}

void ASTargetDummy::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if(Delta < 0.0f)
	{
		meshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}



