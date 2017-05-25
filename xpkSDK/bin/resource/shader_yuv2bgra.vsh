uniform mat4 matViewProjection;		//操作后的矩阵(旋转/平移/缩放/透视投影)
attribute vec4 vertexPosition;
attribute vec2 inputTextureCoordinate;

varying vec2 textureCoordinate;


void main()
{
	gl_Position = matViewProjection*vertexPosition;
	textureCoordinate = inputTextureCoordinate;

	
}
 