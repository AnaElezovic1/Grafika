#version 330 core

struct PositionalLight {
	vec3 Position;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Kc;
	float Kl;
	float Kq;
};

struct DirectionalLight {
	vec3 Position;
	vec3 Direction;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float InnerCutOff;
	float OuterCutOff;
	float Kc;
	float Kl;
	float Kq;
};

struct Material {
	// NOTE(Jovan): Diffuse is used as ambient as well since the light source
	// defines the ambient colour
	sampler2D Kd;
	sampler2D Ks;
	float Shininess;
};

//tackasta
uniform PositionalLight uKamenLight;
uniform PositionalLight uKamenLight1;
uniform PositionalLight uKamenLight2;
uniform PositionalLight uKamenLight3;
uniform PositionalLight uKamenLight4;
uniform PositionalLight uSunceLight;
uniform PositionalLight uMesecLight;

uniform DirectionalLight uSpotlight;
uniform DirectionalLight uDirLight;
uniform DirectionalLight uReflektorLight1;
uniform DirectionalLight uReflektorLight2;
uniform Material uMaterial;
uniform vec3 uViewPos;

in vec2 UV;
in vec3 vWorldSpaceFragment;
in vec3 vWorldSpaceNormal;

out vec4 FragColor;

void main() {
	vec3 ViewDirection = normalize(uViewPos - vWorldSpaceFragment);
	// NOTE(Jovan): Directional light
	vec3 DirLightVector = normalize(-uDirLight.Direction);
	float DirDiffuse = max(dot(vWorldSpaceNormal, DirLightVector), 0.0f);
	vec3 DirReflectDirection = reflect(-DirLightVector, vWorldSpaceNormal);
	// NOTE(Jovan): 32 is the specular shininess factor. Hardcoded for now
	float DirSpecular = pow(max(dot(ViewDirection, DirReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 DirAmbientColor = uDirLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 DirDiffuseColor = uDirLight.Kd * DirDiffuse * vec3(texture(uMaterial.Kd, UV));
	vec3 DirSpecularColor = uDirLight.Ks * DirSpecular * vec3(texture(uMaterial.Ks, UV));
	vec3 DirColor = DirAmbientColor + DirDiffuseColor + DirSpecularColor;



	//ukras1
	// NOTE(Jovan): Point light
	vec3 PtLightVector = normalize(uKamenLight.Position - vWorldSpaceFragment);
	float PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	vec3 PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	float PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 PtAmbientColor = uKamenLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 PtDiffuseColor = PtDiffuse * uKamenLight.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 PtSpecularColor = PtSpecular * uKamenLight.Ks * vec3(texture(uMaterial.Ks, UV));

	float PtLightDistance = length(uKamenLight.Position - vWorldSpaceFragment);
	float PtAttenuation = 1.0f / (uKamenLight.Kc + uKamenLight.Kl * PtLightDistance + uKamenLight.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColor = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);


	// ukras2
	PtLightVector = normalize(uKamenLight1.Position - vWorldSpaceFragment);
	PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	PtAmbientColor = uKamenLight1.Ka * vec3(texture(uMaterial.Kd, UV));
	PtDiffuseColor = PtDiffuse * uKamenLight1.Kd * vec3(texture(uMaterial.Kd, UV));
	PtSpecularColor = PtSpecular * uKamenLight1.Ks * vec3(texture(uMaterial.Ks, UV));

	PtLightDistance = length(uKamenLight1.Position - vWorldSpaceFragment);
	PtAttenuation = 1.0f / (uKamenLight1.Kc + uKamenLight1.Kl * PtLightDistance + uKamenLight1.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColorTorch2 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);

	
	// ukras3
	PtLightVector = normalize(uKamenLight2.Position - vWorldSpaceFragment);
	PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	PtAmbientColor = uKamenLight2.Ka * vec3(texture(uMaterial.Kd, UV));
	PtDiffuseColor = PtDiffuse * uKamenLight2.Kd * vec3(texture(uMaterial.Kd, UV));
	PtSpecularColor = PtSpecular * uKamenLight2.Ks * vec3(texture(uMaterial.Ks, UV));

	PtLightDistance = length(uKamenLight2.Position - vWorldSpaceFragment);
	PtAttenuation = 1.0f / (uKamenLight2.Kc + uKamenLight2.Kl * PtLightDistance + uKamenLight2.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColorTorch3 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);


	// ukras4
	PtLightVector = normalize(uKamenLight3.Position - vWorldSpaceFragment);
	PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	PtAmbientColor = uKamenLight3.Ka * vec3(texture(uMaterial.Kd, UV));
	PtDiffuseColor = PtDiffuse * uKamenLight3.Kd * vec3(texture(uMaterial.Kd, UV));
	PtSpecularColor = PtSpecular * uKamenLight3.Ks * vec3(texture(uMaterial.Ks, UV));

	PtLightDistance = length(uKamenLight3.Position - vWorldSpaceFragment);
	PtAttenuation = 1.0f / (uKamenLight3.Kc + uKamenLight3.Kl * PtLightDistance + uKamenLight3.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColorTorch4 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);

	// ukras5
	PtLightVector = normalize(uKamenLight4.Position - vWorldSpaceFragment);
	PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	PtAmbientColor = uKamenLight4.Ka * vec3(texture(uMaterial.Kd, UV));
	PtDiffuseColor = PtDiffuse * uKamenLight4.Kd * vec3(texture(uMaterial.Kd, UV));
	PtSpecularColor = PtSpecular * uKamenLight4.Ks * vec3(texture(uMaterial.Ks, UV));

	PtLightDistance = length(uKamenLight4.Position - vWorldSpaceFragment);
	PtAttenuation = 1.0f / (uKamenLight4.Kc + uKamenLight4.Kl * PtLightDistance + uKamenLight4.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColorTorch5 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);

	// sunce
	PtLightVector = normalize(uSunceLight.Position - vWorldSpaceFragment);
	PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	PtAmbientColor = uSunceLight.Ka * vec3(texture(uMaterial.Kd, UV));
	PtDiffuseColor = PtDiffuse * uSunceLight.Kd * vec3(texture(uMaterial.Kd, UV));
	PtSpecularColor = PtSpecular * uSunceLight.Ks * vec3(texture(uMaterial.Ks, UV));

	PtLightDistance = length(uSunceLight.Position - vWorldSpaceFragment);
	PtAttenuation = 1.0f / (uSunceLight.Kc + uSunceLight.Kl * PtLightDistance + uSunceLight.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColorSunce = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);


	
	// uReflektorLight1
	vec3 SpotlightVector1 = normalize(uReflektorLight1.Position - vWorldSpaceFragment);

	float SpotDiffuse1 = max(dot(vWorldSpaceNormal, SpotlightVector1), 0.0f);
	vec3 SpotReflectDirection1 = reflect(-SpotlightVector1, vWorldSpaceNormal);
	float SpotSpecular1 = pow(max(dot(ViewDirection, SpotReflectDirection1), 0.0f), uMaterial.Shininess);

	vec3 SpotAmbientColor1 = uReflektorLight1.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotDiffuseColor1 = SpotDiffuse1 * uReflektorLight1.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotSpecularColor1 = SpotSpecular1 * uReflektorLight1.Ks * vec3(texture(uMaterial.Ks, UV));

	float SpotlightDistance1 = length(uReflektorLight1.Position - vWorldSpaceFragment);
	float SpotAttenuation1 = 1.0f / (uReflektorLight1.Kc + uReflektorLight1.Kl * SpotlightDistance1 + uReflektorLight1.Kq * (SpotlightDistance1 * SpotlightDistance1));

	float Theta1 = dot(SpotlightVector1, normalize(-uReflektorLight1.Direction));
	float Epsilon1 = uReflektorLight1.InnerCutOff - uReflektorLight1.OuterCutOff;
	float SpotIntensity1 = clamp((Theta1 - uReflektorLight1.OuterCutOff) / Epsilon1, 0.0f, 1.0f);
	vec3 SpotColor1 = SpotIntensity1 * SpotAttenuation1 * (SpotAmbientColor1 + SpotDiffuseColor1 + SpotSpecularColor1);

		// uReflektorLight2
	vec3 SpotlightVector2 = normalize(uReflektorLight2.Position - vWorldSpaceFragment);

	float SpotDiffuse2 = max(dot(vWorldSpaceNormal, SpotlightVector2), 0.0f);
	vec3 SpotReflectDirection2 = reflect(-SpotlightVector2, vWorldSpaceNormal);
	float SpotSpecular2 = pow(max(dot(ViewDirection, SpotReflectDirection2), 0.0f), uMaterial.Shininess);

	vec3 SpotAmbientColor2 = uReflektorLight2.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotDiffuseColor2 = SpotDiffuse2 * uReflektorLight2.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotSpecularColor2 = SpotSpecular2 * uReflektorLight2.Ks * vec3(texture(uMaterial.Ks, UV));

	float SpotlightDistance2 = length(uReflektorLight2.Position - vWorldSpaceFragment);
	float SpotAttenuation2 = 1.0f / (uReflektorLight2.Kc + uReflektorLight2.Kl * SpotlightDistance2 + uReflektorLight2.Kq * (SpotlightDistance2 * SpotlightDistance2));

	float Theta2 = dot(SpotlightVector2, normalize(-uReflektorLight2.Direction));
	float Epsilon2 = uReflektorLight2.InnerCutOff - uReflektorLight2.OuterCutOff;
	float SpotIntensity2 = clamp((Theta2 - uReflektorLight2.OuterCutOff) / Epsilon2, 0.0f, 1.0f);
	vec3 SpotColor2 = SpotIntensity2 * SpotAttenuation2 * (SpotAmbientColor2 + SpotDiffuseColor2 + SpotSpecularColor2);


	
	
	vec3 FinalColor = DirColor + PtColor  + PtColorTorch2+PtColorTorch3+PtColorTorch4+PtColorTorch5+ PtColorSunce+ SpotColor1+SpotColor2;
	FragColor = vec4(FinalColor, 1.0f);
}
