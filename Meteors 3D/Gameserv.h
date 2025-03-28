#pragma once

#ifdef GAMESERV_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include <random>
#include <d2d1.h>

constexpr float scr_width{ 1000.0f };
constexpr float scr_height{ 800.0f };
constexpr float sky{ 50.0f };
constexpr float ground{ 750.0f };

constexpr uint8_t no_type{ 0 };

constexpr uint8_t type_small_star{ 1 };
constexpr uint8_t type_mid_star{ 2 };
constexpr uint8_t type_big_star{ 4 };

constexpr uint8_t type_meteor1{ 8 };
constexpr uint8_t type_meteor2{ 16 };
constexpr uint8_t type_meteor3{ 32 };

constexpr uint8_t type_left_laser{ 64 };
constexpr uint8_t type_right_laser{ 128 };

struct FPOINT
{
	float x{ 0 };
	float y{ 0 };
};

enum class dirs {
	left = 0, right = 1, up = 2, down = 3, up_left = 4, up_right = 5,
	down_left = 6, down_right = 7, stop = 8
};

namespace dll
{
	class GAME_API RANDIT
	{
	private:

		std::random_device rd{};
		std::seed_seq* sq = nullptr;
		std::mt19937* twister = nullptr;

	public:
		RANDIT();
		~RANDIT();

		int operator() (int min, int max);
	};

	class GAME_API PROTON
	{
	protected:
		float width{ 0 };
		float height{ 0 };

	public:
		FPOINT start{ 0,0 };
		FPOINT end{ 0,0 };
		FPOINT center{ 0,0 };

		float x_radius{ 0 };
		float y_radius{ 0 };

		D2D1_RECT_F Rect{};

		PROTON(float _x, float _y, float _width = 1.0f, float _height = 1.0f);
		virtual ~PROTON() {};

		float GetWidth() const;
		float GetHeight() const;

		void SetWidth(float _new_width);
		void SetHeight(float _new_height);

		void SetEdges();
		void NewDims(float _new_width, float _new_height);
	};

	class GAME_API BASE :public PROTON
	{
	protected:

		float slope{ -1.0f };
		float intercept{ -1.0f };
		float move_sx{ 0 };
		float move_ex{ 0 };
		float move_sy{ 0 };
		float move_ey{ 0 };

		bool hor_line{ false };
		bool vert_line{ false };

		int frame = 0;
		int max_frames = 0;
		int frame_delay = 0;

		float speed{ 0 };

		void SetPath(float _dest_x,float _dest_y);

	public:
		uint8_t type = no_type;
		int lifes{ 0 };
		dirs dir = dirs::stop;

		BASE(uint8_t _what, float __x, float __y, float __to_x = 0, float to_y = 0);
		virtual ~BASE() {};

		int GetFrame();

		virtual bool Move(float gear, bool canvas_move = false, dirs to_where = dirs::stop) = 0;
		virtual void Release() = 0;
	};

	class GAME_API STARS :public BASE
	{
	protected:

		STARS(uint8_t which, float _where_x, float _where_y);

	public:

		friend GAME_API BASE* Factory(uint8_t what, float on_x, float on_y, float to_x, float to_y);

		bool Move(float gear, bool canvas_move, dirs to_where) override;
		void Release()override;
	};

	class GAME_API METEORS :public BASE
	{
	protected:

		METEORS(uint8_t which, float _where_x, float _where_y);

	public:

		friend GAME_API BASE* Factory(uint8_t what, float on_x, float on_y, float to_x, float to_y);

		bool Move(float gear, bool canvas_move, dirs to_where) override;
		void Release()override;
	};

	class GAME_API LASERS :public BASE
	{
	protected:
		LASERS(uint8_t what_laser, float where_x, float where_y, float going_x, float going_y);

	public:
		friend GAME_API BASE* Factory(uint8_t what, float on_x, float on_y, float to_x, float to_y);

		bool Move(float gear, bool canvas_move, dirs to_where) override;
		void Release()override;
	};

	typedef BASE* Object;

	GAME_API BASE* Factory(uint8_t what, float on_x, float on_y, float to_x = 0, float to_y = 0);
}