uniform vec3 color;

float getProceduralFragment(inout ProceduralFragment frag) {
	frag.emissive = color;
	frag.diffuse = color;
	return 0;
}
