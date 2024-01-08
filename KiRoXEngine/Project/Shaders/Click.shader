@vs
layout (location = 0) in vec3 position;

uniform mat4 PVM;

void main(){
    gl_Position = PVM * vec4(position, 1.0);
}
@end
@fs
uniform uint entityIndex;

out uvec3 outColor;

void main(){
    outColor = uvec3(entityIndex, 0, gl_PrimitiveID);
}
@end