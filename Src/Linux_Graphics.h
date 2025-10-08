#include <sys/socket.h>
#include <sys/un.h>
#include <vulkan/vulkan.h>

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
	struct RECT
	{
		INT left;
		INT top;
		INT bottom;
		INT right;
	};
	typedef RECT Rect;

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
	//These buffers hold descriptions of either your constant buffers or samplers.
	class DescriptorBuffer : _APL_DescriptorBuffer
	{

	};

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
	struct ShaderByteCode
	{
		LPVOID pShader;
		UINT64 Size;
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
	
	class Window;
	struct WindowReference
	{
		UINT NoImpl;
		bool operator==(Window w);
	};
	
	struct Window : _APL_Window
	{
	public:
		~Window()
		{
			
		}
		VOID Show(UINT ShowCmd)
		{
			
		}
		VOID Hide(UINT ShowCmd)
		{
			
		}
		VOID SetTitle(LPCWSTR Title)
		{
			
		}
		RECT GetRect()
		{
			return Rect{};
		}
		operator WindowReference()
		{
			//return WindowReference{this->Hwnd};
			return WindowReference{};
		}
		bool operator==(WindowReference wr)
		{
			
			return false;
		}
	};
	bool WindowReference::operator==(Window w)
	{
		
		return false;
	}
	struct WindowDesc
	{
		Rect WindowRect;
		//What would be the HWND equivalant here?
		UINT64 HwndPlaceholder;
		DrawProc pDrawProc;
	};
	

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
		InputType Type;
		union
		{
			struct
			{
				USHORT MakeCode;
				USHORT Flags;
			}Keyboard;
			struct
			{
				USHORT ButtonFlags;
				USHORT ButtonData;
				USHORT LastX;
				USHORT LastY;
			}Mouse;
			WindowReference Window;
		};
	};

	
}

///////////////////////////////////////////////////////////////////////////

INT _APL_GraphicsSetup()
{
	
	return 1;
}

///////////////////////////////////////////////////////////////////////////

namespace APL
{
	struct AdapterInfo
	{

	};
	struct MonitorInfo
	{
		WCHAR Description[32];
		Rect MonitorRect;
		Rect WorkRect;
	};
	class Device : _APL_Device
	{
	public:
		INT NewUploadBuffer(UINT64 Size, UploadBuffer* pUpload)
		{
			
			return 1;
		}
		INT NewObjectBuffer(VertexType Vertex, TopologyLayout Layout, UINT64 TotalVertices, VertexBuffer* pObject)
		{
			return 1;
		}
		INT NewConstantBuffer(UINT64 Size, ConstantBuffer* pConstant)
		{

			return 1;
		}
		INT NewImageBuffer(UINT32 Width, UINT32 Height, ImageBuffer* pImage)
		{
			return 1;
		}
		INT NewConstantDescriptorBuffer(UINT TotalElements, DescriptorBuffer* pDesc)
		{
			return 1;
		}
		INT NewSamplerDescriptorBuffer(UINT TotalElements, DescriptorBuffer* pDesc)
		{
			
			return 1;
		}
		INT NewShaderPack(ShaderPackDesc* pDesc, VertexType Vertex, TopologyType Topology, ShaderPack* pPack)
		{
			

			return 1;
		}
		
		//Pay close attention to the function being used. HANDLE is synonomous to LPVOID so be careful passing them in.
		INT CopyPtrToUpload(UploadBuffer* pDst, LPVOID pSrc, UINT64 Size)
		{
			
			return 1;
		}
		//Pay close attention to the function being used. HANDLE is synonomous to LPVOID so be careful passing them in.
		/*INT CopyFileToUpload(UploadBuffer* pDst, HANDLE hFile, UINT32 Size)
		{
			

			return 1;
		}
		INT CopyFileToUpload(UploadBuffer* pDst, HANDLE hFile, UINT32 Size, UINT64 Offset)
		{
			

			return 1;
		}*/
		INT CopyToObject(VertexBuffer* pDst, UINT64 DstOffset, UploadBuffer* pSrc, UINT64 SrcOffset, UINT32 TotalVertices)
		{
			
			return 1;
		}
		INT CopyToConstant(UploadBuffer* pDst, UINT64 DstOffset, UploadBuffer* pSrc, UINT64 SrcOffset, UINT64 Size)
		{
			
			return 1;
		}
		INT CopyToImage(ImageBuffer* pDst, ImageBuffer* pSrc, UINT Width, UINT Height, UINT xDst, UINT yDst)
		{
			

			return 1;
		}
		INT CopyToImage(ImageBuffer* pDst, UploadBuffer* pSrc, UINT Width, UINT Height, UINT xDst, UINT yDst)
		{
			
			return 1;
		}
		INT ExecuteCopyCommands()
		{
			
			return 1;
		}

		VOID NewConstantDescriptor(ImageBuffer* pImage, UINT Element, DescriptorBuffer* pDesc)
		{
			
		}
		VOID NewSamplerDescriptor(UINT Element, DescriptorBuffer* pDesc)
		{
			
		}

		INT NewWindow(WindowDesc* pDesc, Window* pWindow)
		{
			

			return 1;
		}

		~Device()
		{
			
		}
	};
	class Adapter : _APL_Adapter
	{
	public:
		INT GetAdapterInfo(AdapterInfo* pInfo)
		{
			return 1;
		}
		INT GetMonitorInfo(UINT Monitor, MonitorInfo* pInfo)
		{
			return 1;
		}
		INT NewDevice(Device* pDevice)
		{
			
			return 1;
		}
		~Adapter()
		{

		}
	};
	INT NewAdapter(UINT AdapterIndex, Adapter* pAdapter)
	{
		
		return 1;
	}
	VOID GetInput(InputData* pInput)
	{
		
	}
}