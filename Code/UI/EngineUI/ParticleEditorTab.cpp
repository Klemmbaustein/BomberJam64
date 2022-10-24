#include "ParticleEditorTab.h"
#include <World/Stats.h>
#include <FileUtility.h>
#include <UI/Default/UIButton.h>
#include <UI/Default/VectorInputField.h>
#include <UI/Default/TextField.h>
#include <Log.h>
#include <World/Assets.h>
#include <filesystem>
#include <Rendering/Mesh/Model.h>

ParticleEditorTab::ParticleEditorTab(TextRenderer* Text, GLuint SaveTexture, GLuint ReloadTexture) : EngineTab(Vector2(), Vector2(), Vector3())
{
	if (!ParticleFramebufferObject)
	{
		ParticleFramebufferObject = new FramebufferObject();
		ParticleFramebufferObject->UseMainWindowResolution = true;
		Camera* Cam = new Camera(2, 1600, 900);
		Cam->Position = glm::vec3(15, 0, 40);
		Cam->yaw = 90;
		ParticleFramebufferObject->FramebufferCamera = Cam;
	}
	Particle = new Particles::ParticleEmitter();
	Particle->SetMaterial(0, "NONE");
	ParticleFramebufferObject->ParticleEmitters.push_back(Particle);
	ParticleFramebufferObject->ReInit();
	UIElements.push_back(new UIBorder(Vector2(-0.7f, -0.6f), Vector2(1.4, 1.28f), Vector3(0.1f, 0.1f, 0.105f)));
	TabText = Text;
	UIElements.push_back(new UIBorder(Vector2(-0.7f, -0.6f), Vector2(0.5f, 0.5f), Vector3(1.f), ParticleFramebufferObject->GetTextureID(), true, true));
	UIElements.push_back(new UIBorder(Vector2(-0.7f, -0.1f), Vector2(0.5f, 0.08f), Vector3(0.2f)));
	UIElements.push_back(new UIButton(Vector2(-0.3f, 0.4f), Vector2(0.14f, 0.08f), Vector3(0.4f, 1.f, 0.3f), this, 0));
	UIElements.push_back(new UIButton(Vector2(-0.3f, 0.3f), Vector2(0.14f, 0.08f), Vector3(1.f, 0.2, 0.1f), this, 1));
	Generate();
}

void ParticleEditorTab::Render(Shader* Shader)
{
	ParticleFramebufferObject->FramebufferCamera = Graphics::MainCamera;
	for (int i = 0; i < UIElements.size(); i++)
	{
		UIElements.at(i)->Render(Shader);
	}
	for (int i = 0; i < GeneratedUI.size(); i++)
	{
		GeneratedUI.at(i)->Render(Shader);
	}
	for (int i = 0; i < SettingsButtons.size(); i++)
	{
		SettingsButtons.at(i)->Render(Shader);
	}
	for (unsigned int i = 0; i < Particle->ParticleElements.size(); i++)
	{
		TabText->RenderText("Element " + std::to_string(i), Vector2(-0.63f, 0.44f - (i / 8.f)), 1, 1);
	}

	if (!Particle->IsActive)
	{
		ReactivateDelay -= Performance::DeltaTime;
		if (ReactivateDelay < 0)
		{
			Particle->Reset();
			ReactivateDelay = 1.f;
		}
	}
	TabText->RenderText("Particle System: " + GetFileNameWithoutExtensionFromPath(CurrentSystemFile), Vector2(-0.65f, 0.6f), 1.5f, Vector3(1));
	TabText->RenderText("Preview:", Vector2(-0.68f, -0.08f), 1.2f, Vector3(1));
	TabText->RenderText("Element Count: " + std::to_string(Particle->ParticleElements.size()), Vector2(-0.65f, 0.55f), 1.f, Vector3(1));
	TabText->RenderText("Add Element", Vector2(-0.29f, 0.43f), 1.f, Vector3(0));
	TabText->RenderText("Remove Element", Vector2(-0.29f, 0.33f), 1.f, Vector3(1));
	if (SelectedElement < Particle->ParticleElements.size())
	{
		for (unsigned int i = 0; i < ElementParametersColumn1.size(); i++)
		{
			TabText->RenderText(ElementParametersColumn1[i], Vector2(-0.1f, 0.5f - (0.15f * i)), 1.f, Vector3(1), &ElementScrollObject);
		}
		for (unsigned int i = 0; i < ElementParametersColumn2.size(); i++)
		{
			TabText->RenderText(ElementParametersColumn2[i], Vector2(0.25f, 0.5f - (0.15f * i)), 1.f, Vector3(1), &ElementScrollObject);
		}
	}
}

void ParticleEditorTab::Load(std::string File)
{
	CurrentSystemFile = File;
	Graphics::MainCamera->Position = Vector3(0, 4, 15);
	for (unsigned int i = 0; i < Particle->ParticleVertexBuffers.size(); i++)
	{
		delete Particle->ParticleVertexBuffers[i];
		delete Particle->ParticleIndexBuffers[i];
	}
	Particle->ParticleVertexBuffers.clear();
	Particle->ParticleIndexBuffers.clear();
	Particle->SpawnDelays.clear();
	Particle->ParticleShaders.clear();
	Particle->ParticleInstances.clear();
	Particle->Uniforms.clear();
	Particle->ParticleMatrices.clear();
	Particle->ParticleElements.clear();

	if (std::filesystem::exists(File))
	{
		if (!std::filesystem::is_empty(File))
		{
			auto ParticleData = Particles::ParticleEmitter::LoadParticleFile(File, ElementMaterials);
			for (unsigned int i = 0; i < ParticleData.size(); i++)
			{
				Particle->AddElement(ParticleData[i]);
				Particle->SetMaterial(i, Assets::GetAsset(ElementMaterials[i] + ".jsmat"));
			}
		}
	}
	Generate();
}

void ParticleEditorTab::ReloadMesh()
{
}

void ParticleEditorTab::Save()
{
	Particles::ParticleEmitter::SaveToFile(Particle->ParticleElements, ElementMaterials, CurrentSystemFile);
}

void ParticleEditorTab::Generate()
{
	Particle->Reset();
	for (auto* elem : GeneratedUI)
	{
		delete elem;
	}
	GeneratedUI.clear();

	for (unsigned int i = 0; i < Particle->ParticleElements.size(); i++)
	{
		Vector3 Color = i == SelectedElement ? Vector3(0.3f) : Vector3(0.15f);
		GeneratedUI.push_back(new UIButton(Vector2(-0.65f, 0.4f - (i / 8.f)), Vector2(0.3f, 0.1f), Color, this, 200 + i));
	}

	SettingsButtons.clear();
	if (SelectedElement < Particle->ParticleElements.size())
	{
		auto& SelectedParticle = Particle->ParticleElements[SelectedElement];
		VectorInputField* NewVecField = new VectorInputField(Vector2(-0.1f, 0.4f), 100, TabText, this, SelectedParticle.Direction);
		NewVecField->CurrentScrollObject = &ElementScrollObject;
		SettingsButtons.push_back(NewVecField);

		NewVecField = new VectorInputField(Vector2(-0.1f, 0.25f), 101, TabText, this, SelectedParticle.DirectionRandom);
		NewVecField->CurrentScrollObject = &ElementScrollObject;
		SettingsButtons.push_back(NewVecField);


		TextField* NewTextField = new TextField(Vector2(-0.1f, 0.1f), Vector2(0.3, 0.08f), 0.2, TabText, this, 102, 1);
		NewTextField->CurrentScrollObject = &ElementScrollObject;
		std::stringstream stream;
		stream << std::fixed << std::setprecision(2) << SelectedParticle.Size;
		NewTextField->Text = stream.str();
		SettingsButtons.push_back(NewTextField);

		NewTextField = new TextField(Vector2(-0.1f, -0.05f), Vector2(0.3, 0.08f), 0.2, TabText, this, 103, 1);
		NewTextField->CurrentScrollObject = &ElementScrollObject;
		stream = std::stringstream();
		stream << std::fixed << std::setprecision(2) << SelectedParticle.LifeTime;
		NewTextField->Text = stream.str();
		SettingsButtons.push_back(NewTextField);

		NewTextField = new TextField(Vector2(-0.1f, -0.2f), Vector2(0.3, 0.08f), 0.2, TabText, this, 104, 1);
		NewTextField->CurrentScrollObject = &ElementScrollObject;
		stream = std::stringstream();
		stream << std::fixed << std::setprecision(2) << SelectedParticle.SpawnDelay;
		NewTextField->Text = stream.str();
		SettingsButtons.push_back(NewTextField);

		NewTextField = new TextField(Vector2(-0.1f, -0.35f), Vector2(0.3, 0.08f), 0.2, TabText, this, 105, 1);
		NewTextField->CurrentScrollObject = &ElementScrollObject;
		NewTextField->Text = std::to_string(SelectedParticle.NumLoops);
		SettingsButtons.push_back(NewTextField);

		NewTextField = new TextField(Vector2(-0.1f, -0.5f), Vector2(0.3, 0.08f), 0.2, TabText, this, 106, 1);
		NewTextField->CurrentScrollObject = &ElementScrollObject;
		NewTextField->Text = ElementMaterials[SelectedElement];
		SettingsButtons.push_back(NewTextField);

		NewVecField = new VectorInputField(Vector2(0.25f, 0.4f), 107, TabText, this, SelectedParticle.PositionRandom);
		NewVecField->CurrentScrollObject = &ElementScrollObject;
		SettingsButtons.push_back(NewVecField);

		NewVecField = new VectorInputField(Vector2(0.25f, 0.25f), 108, TabText, this, SelectedParticle.Force);
		NewVecField->CurrentScrollObject = &ElementScrollObject;
		SettingsButtons.push_back(NewVecField);

		NewTextField = new TextField(Vector2(0.25f, 0.1f), Vector2(0.3, 0.08f), 0.2, TabText, this, 109, 1);
		NewTextField->CurrentScrollObject = &ElementScrollObject;
		NewTextField->Text = std::to_string(SelectedParticle.StartScale);
		SettingsButtons.push_back(NewTextField);

		NewTextField = new TextField(Vector2(0.25f, -0.05f), Vector2(0.3, 0.08f), 0.2, TabText, this, 110, 1);
		NewTextField->CurrentScrollObject = &ElementScrollObject;
		NewTextField->Text = std::to_string(SelectedParticle.EndScale);
		SettingsButtons.push_back(NewTextField);
	}
}

void ParticleEditorTab::OnButtonClicked(int Index)
{
	try
	{
		if (Index == 0)
		{
			ElementMaterials.push_back("NONE");
			Particle->AddElement(Particles::ParticleElement());
			Generate();
			return;
		}
		if (Index == 1)
		{
			Particle->RemoveElement(SelectedElement);
			Generate();
			return;
		}
		if (Index >= 200 && Index < 300)
		{
			SelectedElement = Index - 200;
			Generate();
			return;
		}
		if (Index == 100)
		{
			Particle->ParticleElements[SelectedElement].Direction = ((VectorInputField*)SettingsButtons[Index - 100])->GetValue();
			Generate();
			return;
		}
		if (Index == 101)
		{
			Particle->ParticleElements[SelectedElement].DirectionRandom = ((VectorInputField*)SettingsButtons[Index - 100])->GetValue();
			Generate();
			return;
		}
		if (Index == 102)
		{
			Particle->ParticleElements[SelectedElement].Size = std::stof(((TextField*)SettingsButtons[Index - 100])->Text);
			Generate();
			return;
		}
		if (Index == 103)
		{
			Particle->ParticleElements[SelectedElement].LifeTime = std::stof(((TextField*)SettingsButtons[Index - 100])->Text);
			Generate();
			return;
		}
		if (Index == 104)
		{
			Particle->ParticleElements[SelectedElement].SpawnDelay = std::stof(((TextField*)SettingsButtons[Index - 100])->Text);
			//Generate();
			return;
		}
		if (Index == 105)
		{
			Particle->ParticleElements[SelectedElement].NumLoops = std::stoi(((TextField*)SettingsButtons[Index - 100])->Text);
			Generate();
			return;
		}
		if (Index == 106)
		{
			Particle->SetMaterial(SelectedElement, Assets::GetAsset(((TextField*)SettingsButtons[Index - 100])->Text + ".jsmat"));
			ElementMaterials[SelectedElement] = ((TextField*)SettingsButtons[Index - 100])->Text;
			Generate();
			return;
		}
		if (Index == 107)
		{
			Particle->ParticleElements[SelectedElement].PositionRandom = ((VectorInputField*)SettingsButtons[Index - 100])->GetValue();
			Generate();
			return;
		}
		if (Index == 108)
		{
			Particle->ParticleElements[SelectedElement].Force = ((VectorInputField*)SettingsButtons[Index - 100])->GetValue();
			Generate();
			return;
		}
		if (Index == 109)
		{
			Particle->ParticleElements[SelectedElement].StartScale = std::stof(((TextField*)SettingsButtons[Index - 100])->Text);
			Generate();
			return;
		}
		if (Index == 110)
		{
			Particle->ParticleElements[SelectedElement].EndScale = std::stof(((TextField*)SettingsButtons[Index - 100])->Text);
			Generate();
			return;
		}
	}
	catch (std::exception& e)
	{
		Log::CreateNewLogMessage(e.what());
	}
}

ParticleEditorTab::~ParticleEditorTab()
{
}
