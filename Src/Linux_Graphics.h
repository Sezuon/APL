struct _APL_Adapter
{

};
struct _APL_Device
{

};
struct _APL_UploadBuffer
{

};
struct _APL_VertexBuffer
{

};
struct _APL_ConstantBuffer
{

};
struct _APL_ImageBuffer
{

};
struct _APL_DescriptorBuffer
{

};
struct _APL_ShaderPack
{

};
struct _APL_GraphicsContext
{

};
struct _APL_Window
{

};

namespace APL
{
	struct Rect
	{
		INT left;
		INT top;
		INT bottom;
		INT right;
	};

	struct ShaderByteCode
	{
		LPVOID pShader;
		UINT64 Size;
	};

	enum VertexType
	{
		Vert16,
		Vert24,
		Vert32,
	};
	struct Vertex16
	{
		FLOAT Pos[4];
	};
	struct Vertex24
	{
		FLOAT Pos[4];
		FLOAT UV[2];
	};

	class UploadBuffer : _APL_UploadBuffer
	{
	
	};
	class VertexBuffer : _APL_VertexBuffer
	{

	};
	class ConstantBuffer : _APL_ConstantBuffer
	{

	};
	class ImageBuffer : _APL_ImageBuffer
	{

	};
	class DescriptorBuffer : _APL_DescriptorBuffer
	{

	};
	
	//Is this even used on the Win32 side?
	typedef LPVOID DescriptorHandle;

	//Don't know what values are needed here just yet.
	enum TopologyType
	{
		Point,
		Line,
		Triangle,
	};
	//also don't know what these should be yet either.
	enum TopologyLayout
	{
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
	};
	struct ShaderPackDesc
	{
		ShaderByteCode VS;
		ShaderByteCode PS;
		ShaderByteCode DS;
		ShaderByteCode HS;
		ShaderByteCode GS;
	};
	struct ShaderPack : _APL_ShaderPack
	{

	};
	struct ViewPort
	{
		INT32 xPos;
		INT32 yPos;
		INT32 Width;
		INT32 Height;
		INT32 MinDepth;
		INT32 MaxDepth;
	};
	class GraphicsContext : _APL_GraphicsContext
	{
		public:
			VOID NoImpl()
			{
			}
	};

	typedef VOID(*DrawProc)(GraphicsContext);
	
	enum InputType
	{
		Keyboard, Mouse, Close
	};
	enum MouseFlags
	{
		//Impl later.
	};
	struct InputData
	{
		//Impl later.
	};

	struct WindowDesc
	{
		Rect WindowRect;
		//What would be the HWND equivalant here?
		UINT64 HwndPlaceholder;
		DrawProc pDrawProc;
	};
	struct Window : _APL_Window
	{
	public:

	};
}

///////////////////////////////////////////////////////////////////////////

INT _APL_GraphicsSetup()
{
	LPCHAR XdgRuntimeDir = std::getenv("XDG_RUNTIME_DIR");
	if(XdgRuntimeDir == 0)
		return 1;

	UINT XdgRuntimeDirLength{};
	for(; XdgRuntimeDir[XdgRuntimeDirLength++] != *"\0";)
		;

	sockaddr_un Addr{AF_UNIX};
	if(XdgRuntimeDirLength > sizeof(Addr.sun_path))
		return 1;

	memcpy(Addr.sun_path, XdgRuntimeDir, XdgRuntimeDirLength);
	
	return 1;
}