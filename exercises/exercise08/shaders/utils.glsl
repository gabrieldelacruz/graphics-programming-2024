
// Returns camera position, extracted from view matrix
vec3 GetCameraPosition(mat4 viewMatrix)
{
	vec3 position = viewMatrix[3].xyz;
	position = -(transpose(viewMatrix) * vec4(position, 0)).xyz;
	return position;
}

// Gets vector halfway between two other vectors (typically, light and view)
vec3 GetHalfVector(vec3 v1, vec3 v2)
{
   return normalize(v1 + v2);
}

// Gets the direction from a point to another
vec3 GetDirection(vec3 fromPosition, vec3 toPosition)
{
	return normalize(toPosition - fromPosition);
}

// Returns the dot product, clamped only to positive values
float ClampedDot(vec3 v1, vec3 v2)
{
	return max(dot(v1, v2), 0);
}

// Constructs the 3D normal using only XY values (computing implicit Z)
vec3 GetImplicitNormal(vec2 normal)
{
	float z = sqrt(1.0f - normal.x * normal.x - normal.y * normal.y);
	return vec3(normal, z);
}

// Sample texture map in tangent space and converts to the same space of the provided normal, tangent and bitangent 
vec3 SampleNormalMap(sampler2D normalTexture, vec2 texCoord, vec3 normal, vec3 tangent, vec3 bitangent)
{
	// Read normalTexture
	vec2 normalMap = texture(normalTexture, texCoord).xy * 2 - vec2(1);

	// Get implicit Z component
	vec3 normalTangentSpace = GetImplicitNormal(normalMap);

	// Create tangent space matrix
	mat3 tangentMatrix = mat3(tangent, bitangent, normal);

	// Return matrix in world space
	return normalize(tangentMatrix * normalTangentSpace);
}

// Sample texture map in tangent space and converts to the same space of the provided normal and tangent 
vec3 SampleNormalMap(sampler2D normalTexture, vec2 texCoord, vec3 normal, vec3 tangent)
{
	// Build the tangent space base vectors
	normal = normalize(normal);
	vec3 bitangent = normalize(cross(normal, tangent));
	tangent = cross(normal, bitangent);

	return SampleNormalMap(normalTexture, texCoord, normal, tangent, bitangent);
}

// Obtains a position in view space using the depth buffer and the inverse projection matrix
vec3 ReconstructViewPosition(sampler2D depthTexture, vec2 texCoord, mat4 invProjMatrix)
{
	// Reconstruct the position, using the screen texture coordinates and the depth
	float depth = texture(depthTexture, texCoord).r;
	vec3 clipPosition = vec3(texCoord, depth) * 2.0f - vec3(1.0f);
	vec4 viewPosition = invProjMatrix * vec4(clipPosition, 1.0f);
	return viewPosition.xyz / viewPosition.w;
}
