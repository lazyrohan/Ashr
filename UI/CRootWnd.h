#pragma once

#include <Windows.h>
#include <atlstr.h>
//Basic function header
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <vector>
//Driect2D libs
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
//Image codec lib 
#include <wincodec.h>

#pragma comment(lib,"d2d1")
#pragma comment(lib,"dwrite")
#pragma comment(lib,"windowscodecs.lib")

namespace Ashr
{
	namespace UI
	{
		//Debug info
#ifdef DEBUG
		inline void DebugInfo() {}
#else
		inline void DebugInfo() {}
#endif

		//Safe release function. need?
		template <class Interface>
		inline void SafeRelease(Interface** ppInterfaceToRelease)
		{
			if (*ppInterfaceToRelease != NULL)
			{
				(*ppInterfaceToRelease)->Release();
				(*ppInterfaceToRelease) = NULL;
			}
		};

		//Debug output
#ifndef Assert
#if defined(DEBUG) || defined(_DEBUG)
#define Assert(b) do { if(!b){OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else 
#define Assert(b)
#endif
#endif

/*
exe 文件 被 系统 装入 内存 后 叫  IMAGE 而 instance 从 DOS_HEADER 位置 开始。
所以
IMAGE_DOS_HEADER __ImageBase;
(HINSTANCE)&__ImageBase // instance 从 DOS_HEADER 位置 开始
*/
#ifndef HINST_THISCOMPONENT
		EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

		template <typename ChildWnd>
		class CRootWnd
		{
		public:
			CRootWnd() :
				mHwnd(NULL),
				mpFactory(NULL),
				mpWICFactory(NULL),
				mpWriteFactory(NULL),
				mpTextFmt(NULL),
				mpRendertarget(NULL),
				mpBitmap(NULL),
				mMousePos(D2D1::Point2F()),
				mMouseMoveRC(D2D1::RectF())
			{
				for (auto itr : mpBrushList)
					itr = NULL;
			};
			virtual ~CRootWnd()
			{
				ReleaseDevRes();
				//SafeRelease(&mpBitmap);
				SafeRelease(&mpFactory);
				//SafeRelease(&mpWICFactory);
				mHwnd = NULL;
			};

		public:
			//Initialize window and load respurce
			virtual HRESULT Initialize(HWND hParentWnd = NULL)
			{
				HRESULT hr = S_OK;
				hr = CreateIndependRes();
				if (FAILED(hr))
					return hr;

				// Check show window
				if (hParentWnd == NULL)
				{
					hr = CreateWnd(L"Show");
					if (FAILED(hr))
						return hr;
				}
				else
					mHwnd = hParentWnd;

				return hr;
			};

			//Main process to handle message
			int Run()
			{
				MSG msg;

				while (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				return (int)msg.wParam;
			};

			virtual LRESULT MsgHandle(UINT msg, WPARAM wpm, LPARAM lpm)
			{
				switch (msg)
				{
				case WM_SIZE:
				{
					OnResize(lpm);
					InvalidateRect(mHwnd, NULL, false);
				}
				break;

				case WM_DISPLAYCHANGE:
				{
					InvalidateRect(mHwnd, NULL, false);
				}
				break;

				case WM_PAINT:
				{
					//MessageBox(NULL, L"Hi there paint", L"GO", MB_YESNO);
					OnDrawing(mHwnd);
					ValidateRect(mHwnd, NULL);
				}
				break;

				case WM_NCPAINT:
				{
				}

				case WM_DESTROY:
				{
					PostQuitMessage(0);
				}
				break;

				case WM_MOUSEMOVE:
				{
					OnMouseMove(lpm, wpm);
				}
				break;

				case WM_LBUTTONDOWN:
				{
					OnLbtnDown(lpm, wpm);
				}
				break;

				case WM_LBUTTONUP:
				{
					OnLbtnUp();
				}
				break;

				default:
					return ::DefWindowProcW(mHwnd, msg, wpm, lpm);

				}
				return 0;
			};

			//Resource created for each scene, open public interface for use
			virtual HRESULT CreateDevDependRes(HWND hwd)
			{
				if (!hwd)
				{
					MessageBoxW(NULL, L"Null HWND to create rendertarget resource ", L"ParameterWrong", NULL);
					return S_FALSE;
				}
				HRESULT hr = S_OK;

				if (!mpRendertarget)
				{
					//MessageBox(NULL, L"Invalid render target", L"ParameterWrong", MB_YESNO);
					RECT rc;
					GetClientRect(hwd, &rc);
					D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

					hr = mpFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwd, size), &mpRendertarget);
				}

				if (SUCCEEDED(hr))
				{
					//Create brush
					ID2D1SolidColorBrush* pbrush = NULL;
					hr = mpRendertarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0.0f, 1.0f, 1.0f, 1.0f)), &pbrush);
					mpBrushList.push_back(pbrush);
					/*ID2D1LinearGradientBrush* gbrush = NULL;
					ID2D1GradientStopCollection* pgstop = NULL;
					D2D1_GRADIENT_STOP gstop[3];
					gstop[0].color = D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f);
					gstop[0].position = 0.0f;
					gstop[1].color = D2D1::ColorF(0.0f, 1.0f, 0.0f, 1.0f);
					gstop[1].position = 0.5f;
					gstop[2].color = D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f);
					gstop[2].position = 1.0f;
					hr = mpRendertarget->CreateGradientStopCollection(gstop, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pgstop);
					if (SUCCEEDED(hr))
						hr = mpRendertarget->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0.0f, 0.0f), D2D1::Point2(100.0f, 100.0f)), pgstop, &gbrush);*/
					hr = mpRendertarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)), &pbrush);
					mpBrushList.push_back(pbrush);
					if (FAILED(hr))
						MessageBox(NULL, L"Create brush failed", L"ParameterWrong", MB_YESNO);
					ID2D1Image* pImg = NULL;
				}

				return hr;
			};

			virtual HRESULT CreateIndependRes(void)
			{
				HRESULT hr = S_OK;
				/*
			Initialize D2D
			1. Create an ID2D1Factory1
			2. Create an ID2D1Device and an ID2D1DeviceContext
			*/
			//create render factory
				hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mpFactory);

				if (FAILED(hr))
				{
					MessageBox(NULL, L"Create factory failed", L"Oohoo wrong", MB_OK);
					return hr;
				}

				//create Image factory
				hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)& mpWICFactory);
				if (FAILED(hr))
				{
					MessageBox(NULL, L"Create WICImagingfactory failed", L"Oohoo wrong", MB_OK);
					return hr;
				}

				//create write factory
				hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown * *> (&mpWriteFactory));
				if (FAILED(hr))
				{
					MessageBox(NULL, L"Create IDwriteFactory failed", L"Oohoo wrong", MB_OK);
					return hr;
				}

				hr = mpWriteFactory->CreateTextFormat(L"Calibri", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 18.0f, L"en-us", &mpTextFmt);
				if (SUCCEEDED(hr))
				{
					hr = mpTextFmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				}

				if (SUCCEEDED(hr))
				{
					hr = mpTextFmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				}
				return hr;
			};

			// Load image
			HRESULT LoadImgFromFile(LPCWSTR filename)
			{
				HRESULT hr = S_OK;

				if (!mpWICFactory)
				{
					hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)& mpWICFactory);
				}

				//1. Create WIC decoder  
				IWICBitmapDecoder* pDecoder = NULL;
				if (SUCCEEDED(hr))
				{
					hr = mpWICFactory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);
				}

				//2. Retrieve a frame
				IWICBitmapFrameDecode* pFrmDecode = NULL;
				if (SUCCEEDED(hr))
				{
					hr = pDecoder->GetFrame(0, &pFrmDecode);
				}
				//3. Covert to D2D format
				IWICFormatConverter* pConverter = NULL;
				if (SUCCEEDED(hr))
				{
					hr = mpWICFactory->CreateFormatConverter(&pConverter);
				}

				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(pFrmDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
				}

				if (SUCCEEDED(hr))
				{
					mpRendertarget->CreateBitmapFromWicBitmap(pConverter, &mpBitmap);
				}
				SafeRelease(&pConverter);
				SafeRelease(&pFrmDecode);
				SafeRelease(&pDecoder);
				return hr;
			};

		protected:
			//Create default window if not exist 
			virtual HRESULT CreateWnd(LPCWSTR WndName)
			{
				HRESULT hr = S_OK;

				// register windows class
				WNDCLASSEXW wce = {};
				wce.cbSize = sizeof(WNDCLASSEX);
				wce.style = CS_HREDRAW | CS_VREDRAW;
				wce.lpfnWndProc = CRootWnd::WndProc;
				wce.cbClsExtra = 0;
				wce.cbWndExtra = 0;
				wce.hInstance = GetModuleHandleW(NULL);
				wce.hbrBackground = NULL;
				wce.lpszMenuName = NULL;
				wce.hCursor = LoadCursor(NULL, IDI_APPLICATION);
				wce.lpszClassName = WndName;

				RegisterClassEx(&wce);

				//Get system screen size
				RECT sysrc;
				//Used for set system enviroment
				SystemParametersInfoW(SPI_GETWORKAREA, 0, &sysrc, 0);
				int startx = (int)((sysrc.right - sysrc.left) * 0.125);
				int starty = sysrc.top;
				//Create window
				mHwnd = CreateWindowExW(
					0,
					L"ShowWindow",
					L"Show window",
					WS_OVERLAPPEDWINDOW,
					startx, starty,
					(int)((sysrc.right - sysrc.left) * 0.75), sysrc.bottom - sysrc.top,
					NULL,
					NULL,
					HINST_THISCOMPONENT,
					this);

				//Show window
				hr = mHwnd ? S_OK : E_FAIL;
				if (SUCCEEDED(hr))
				{
					//SetWindowLongW(mHwnd, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
					ShowWindow(mHwnd, SW_SHOWNORMAL);
					UpdateWindow(mHwnd);
				}

				return hr;
			};

			HRESULT OnDrawing(HWND hwd)
			{
				HRESULT hr = S_OK;
				//ID2D1HwndRenderTarget* mpRendertarget = NULL;

				if (!mpRendertarget)
				{
					hr = CreateDevDependRes(mHwnd);
				}


				//Create resource
				if (SUCCEEDED(hr))
				{

					mpRendertarget->BeginDraw();
					mpRendertarget->Clear(D2D1::ColorF(0.0f, 0.05f, 0.0f, 1.0f)); //[0.0 1.0]
					mpRendertarget->FillRectangle(D2D1::RectF(210.0f, 210.0f, 400.0f, 400.0f), mpBrushList[0]);

					for (int i = 0; i < 40; i++)
					{
						mpRendertarget->DrawRectangle(D2D1::RectF(10.0f + i * 10, 10.0f + i * 10, 600.0f - i * 10, 600.0f - i * 10), mpBrushList[1], 5.0f);
					}

					// Draw pics
					// Load pics
					hr = LoadImgFromFile(L"D:\\prog\\steam\\steamapps\\common\\Factorio\\data\\base\\graphics\\entity\\electric-furnace\\hr-electric-furnace.png");
					if (SUCCEEDED(hr))
					{
						float scaler = 1.0f;
						D2D1_SIZE_F imgSize = mpBitmap->GetSize();
						D2D1_RECT_F srcrect = D2D1::RectF(0, 0, imgSize.width, imgSize.height);
						D2D1_SIZE_F csize = mpRendertarget->GetSize();
						for (int j = 0; j < csize.height / (imgSize.height * scaler); j++)
						{
							for (int i = 0; i < csize.width / (imgSize.width * scaler); i++)
							{
								srcrect = D2D1::RectF(i * imgSize.width * scaler, j * imgSize.height * scaler, (i + 1) * imgSize.width * scaler, (1 + j) * imgSize.height * scaler);
								mpRendertarget->DrawBitmap(mpBitmap, srcrect);
								mpRendertarget->DrawRectangle(srcrect, mpBrushList[1], 0.5f);
							}
						}
					}
				}

				return hr;
			};

			HRESULT OnResize(LPARAM lpm)
			{
				HRESULT hr = S_OK;
				if (mpRendertarget)
				{
					hr = mpRendertarget->Resize(D2D1::SizeU(LOWORD(lpm), HIWORD(lpm)));
				}

				return hr;
			};

			HRESULT OnMouseMove(LPARAM lpm, WPARAM wpm)
			{
				HRESULT hr = S_OK;

				// Get mouse position


				if (wpm & MK_LBUTTON)
				{
					D2D1_POINT_2F curPos = CaptureMousePos(lpm);
					mMouseMoveRC = D2D1::RectF(mMousePos.x, mMousePos.y, curPos.x, curPos.y);
					InvalidateRect(mHwnd, NULL, FALSE);
				}

				return hr;
			};

			HRESULT OnLbtnDown(LPARAM lpm, WPARAM wpm)
			{
				HRESULT hr = S_OK;
				SetCapture(mHwnd);
				mMousePos = CaptureMousePos(lpm);
				InvalidateRect(mHwnd, NULL, FALSE);

				return hr;
			};

			void OnLbtnUp()
			{
				ReleaseCapture();
			};

			void ReleaseDevRes(void)
			{
				for (auto itr : mpBrushList)
					SafeRelease(&itr);
				mpBrushList.clear();
				//SafeRelease(&mpBitmap);
				SafeRelease(&mpRendertarget);
			};

			void ReleaseIndependRes(void)
			{
				SafeRelease(&mpFactory);
				SafeRelease(&mpWriteFactory);
				SafeRelease(&mpTextFmt);
			};

			D2D1_POINT_2F CaptureMousePos(LPARAM lpm)
			{
				D2D1_POINT_2F mpos = D2D1::Point2F();
				if (mpFactory)
				{
					FLOAT dpix = 0.0f;
					FLOAT dpiy = 0.0f;
					mpFactory->GetDesktopDpi(&dpix, &dpiy);
					mpos = D2D1::Point2F(((float)(short)LOWORD(lpm)) * 96.0f / dpix, ((float)(short)HIWORD(lpm)) * 96.0f / dpiy);

				}
				return mpos;
			};

		private:
			//Message process
			static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wpm, LPARAM lpm)
			{
				ChildWnd* pthis = NULL;
				if (msg == WM_NCCREATE)
				{
					LPCREATESTRUCT pcs = (LPCREATESTRUCT)lpm;
					pthis = (ChildWnd*)pcs->lpCreateParams;

					::SetWindowLongPtrW(
						hwnd,
						GWLP_USERDATA,
						PtrToUlong(pthis)
					);
					pthis->mHwnd = hwnd;
				}
				else
				{
					pthis = reinterpret_cast<ChildWnd*>(static_cast<LONG_PTR>(
						::GetWindowLongPtrW(
							hwnd,
							GWLP_USERDATA
						)));
				}

				if (pthis)
				{
					return pthis->MsgHandle(msg, wpm, lpm);
				}
				else
				{
					return ::DefWindowProc(hwnd, msg, wpm, lpm);
				}
			};


		private:
			HWND mHwnd;
			ID2D1Factory* mpFactory;
			IWICImagingFactory* mpWICFactory;
			IDWriteFactory* mpWriteFactory;
			IDWriteTextFormat* mpTextFmt;
			ID2D1HwndRenderTarget* mpRendertarget;
			ID2D1Bitmap* mpBitmap;
			std::vector < ID2D1SolidColorBrush*> mpBrushList;
			D2D1_POINT_2F mMousePos;
			D2D1_RECT_F mMouseMoveRC;
		};
	}
}

