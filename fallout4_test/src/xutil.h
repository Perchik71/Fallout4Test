#pragma once

#pragma warning(disable:4094) // untagged 'struct' declared no symbols
#pragma warning(disable:4172)

#include <atomic>
#include <algorithm>
#include <functional>
#include <string>
#include <future>
#include <locale>

#define Assert(Cond)					if(!(Cond)) XUtil::XAssert(__FILE__, __LINE__, #Cond);
#define AssertDebug(Cond)				if(!(Cond)) XUtil::XAssert(__FILE__, __LINE__, #Cond);
#define AssertMsgVa(Cond, Msg, ...)		if(!(Cond)) XUtil::XAssert(__FILE__, __LINE__, "%s\n\n" Msg, #Cond, ##__VA_ARGS__);
#define AssertMsg(Cond, Msg)			AssertMsgVa(Cond, Msg)
#define AssertMsgDebug(Cond, Msg)		AssertMsgVa(Cond, Msg)

#define templated(...)					__VA_ARGS__
#define AutoPtr(Type, Name, Offset)		static Type& Name = (*(Type *)((uintptr_t)GetModuleHandle(nullptr) + Offset))
#define AutoFunc(Type, Name, Offset)	static auto Name = ((Type)((uintptr_t)GetModuleHandle(nullptr) + Offset))

#define static_assert_offset(Structure, Member, Offset) struct __declspec(empty_bases) : CheckOffset<offsetof(Structure, Member), Offset> { }
#define assert_vtable_index(Function, Index) AssertMsgVa(VtableIndexUtil::GetIndexOf(Function) == Index, "Virtual table index does not match (%d != %d)", VtableIndexUtil::GetIndexOf(Function), Index)

#define GAME_TLS(Type, Offset) *(Type *)(*(uintptr_t *)(__readgsqword(0x58u) + 8i64 * (*(uint32_t *)(g_ModuleBase + 0x34BBA78))) + (Offset))

#define STATIC_CONSTRUCTOR(Id, Lambda) \
	struct \
	{ \
		static void Id() \
		{ \
			StaticConstructor<&Id>::C; \
			Lambda(); \
		} \
	};

#define DECLARE_CONSTRUCTOR_HOOK(Class) \
	static Class *__ctor__(void *Instance) \
	{ \
		return new (Instance) Class(); \
	} \
	\
	static Class *__dtor__(Class *Thisptr, unsigned __int8) \
	{ \
		Thisptr->~Class(); \
		return Thisptr; \
	}

class VtableIndexUtil
{
private:
	typedef int(*VtableIndexFn)();
	static VtableIndexUtil *GlobalInstance;

public:
	static VtableIndexUtil *Instance();

	template<typename T>
	static int GetIndexOf(T ptr)
	{
		return (Instance()->**((decltype(&ForceVtableReference)*)(&ptr)))();
	}

private:
	virtual int ForceVtableReference();
};

template<void(*ctor)()>
struct StaticConstructor
{
	struct Constructor
	{
		Constructor()
		{
			ctor();
		}
	};

	static Constructor C;
};

template<void(*ctor)()>
typename StaticConstructor<ctor>::Constructor StaticConstructor<ctor>::C;

template <size_t Offset, size_t RequiredOffset>
struct __declspec(empty_bases)CheckOffset
{
	static_assert(Offset <= RequiredOffset, "Offset is larger than expected");
	static_assert(Offset >= RequiredOffset, "Offset is smaller than expected");
};

namespace XUtil
{
	namespace Parallel
	{
		// https://stackoverflow.com/questions/40805197/parallel-for-each-more-than-two-times-slower-than-stdfor-each
		// fast than tbb::parallel_for_each

		class join_threads
		{
		public:
			explicit join_threads(std::vector<std::thread>& threads)
				: threads_(threads) {}

			~join_threads()
			{
				for (size_t i = 0; i < threads_.size(); ++i)
				{
					if (threads_[i].joinable())
					{
						threads_[i].join();
					}
				}
			}

		private:
			std::vector<std::thread>& threads_;
		};

		template<typename Iterator, typename Func>
		void for_each(Iterator first, Iterator last, Func func)
		{
			const auto length = std::distance(first, last);
			if (0 == length) return;

			const auto min_per_thread = 25u;
			const unsigned max_threads = (length + min_per_thread - 1) / min_per_thread;
			const auto hardware_threads = std::thread::hardware_concurrency();
			const auto num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2u, max_threads);
			const auto block_size = length / num_threads;

			std::vector<std::future<void>> futures(num_threads - 1);
			std::vector<std::thread> threads(num_threads - 1);
			join_threads joiner(threads);

			auto block_start = first;
			for (unsigned i = 0; i < num_threads - 1; ++i)
			{
				auto block_end = block_start;
				std::advance(block_end, block_size);
				std::packaged_task<void(void)> task([block_start, block_end, func]()
					{
						std::for_each(block_start, block_end, func);
					});
				futures[i] = task.get_future();
				threads[i] = std::thread(std::move(task));
				block_start = block_end;
			}

			std::for_each(block_start, last, func);

			for (size_t i = 0; i < num_threads - 1; ++i)
				futures[i].get();
		}
	}

	namespace Str
	{
		// https://thispointer.com/implementing-a-case-insensitive-stringfind-in-c/

		/*
		 * Find Case Insensitive Sub String in a given substring
		 */

		size_t findCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0);

		// trim https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

		// trim from start (in place)
		static inline std::string ltrim(std::string& s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
			return s;
		}

		static inline std::string ltrim(const std::string& s) {
			return ltrim(const_cast<std::string&>(s));
		}

		// trim from end (in place)
		static inline std::string rtrim(std::string& s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
			return s;
		}

		static inline std::string rtrim(const std::string& s) {
			return rtrim(const_cast<std::string&>(s));
		}

		// trim from both ends
		static inline std::string& trim(std::string& s) {
			return ltrim(rtrim(s));
		}

		static inline std::string trim(const std::string& s) {
			return trim(const_cast<std::string&>(s));
		}

		// convert string to upper case
		static inline std::string& UpperCase(std::string& s) {
			std::for_each(s.begin(), s.end(), [](char& c) {
				c = ::toupper(c);
				});
			return s;
		}

		// convert string to upper case
		static inline std::string& LowerCase(std::string& s) {
			std::for_each(s.begin(), s.end(), [](char& c) {
				c = ::tolower(c);
				});
			return s;
		}

		// https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
		std::string __stdcall GetLastErrorToStr(DWORD err, const std::string& namefunc);
		std::string __stdcall GetLastErrorToStr(const std::string& namefunc);

		// formating string
		std::string format(const char* fmt, ...);
	}

	void SetThreadName(uint32_t ThreadID, const char *ThreadName);
	void Trim(char *Buffer, char C);
	void XAssert(const char *File, int Line, const char *Format, ...);
	uint64_t MurmurHash64A(const void *Key, size_t Len, uint64_t Seed = 0);

	uintptr_t FindPattern(uintptr_t StartAddress, uintptr_t MaxSize, const char *Mask);
	std::vector<uintptr_t> FindPatterns(uintptr_t StartAddress, uintptr_t MaxSize, const char *Mask);
	bool GetPESectionRange(uintptr_t ModuleBase, const char *Section, uintptr_t *Start, uintptr_t *End);
	void PatchMemory(uintptr_t Address, uint8_t *Data, size_t Size);
	void PatchMemory(uintptr_t Address, std::initializer_list<uint8_t> Data);
	void PatchMemoryNop(uintptr_t Address, size_t Size);
	void DetourJump(uintptr_t Target, uintptr_t Destination);
	void DetourCall(uintptr_t Target, uintptr_t Destination);

	template<typename T>
	void DetourJump(uintptr_t Target, T Destination)
	{
		static_assert(std::is_member_function_pointer_v<T> || (std::is_pointer_v<T> && std::is_function_v<typename std::remove_pointer<T>::type>));

		DetourJump(Target, *(uintptr_t *)&Destination);
	}

	template<typename T>
	void DetourCall(uintptr_t Target, T Destination)
	{
		static_assert(std::is_member_function_pointer_v<T> || (std::is_pointer_v<T> && std::is_function_v<typename std::remove_pointer<T>::type>));

		DetourCall(Target, *(uintptr_t *)&Destination);
	}

	template <typename T>
	void __stdcall DetourClassCall(uintptr_t target, T destination)
	{
		Detours::X64::DetourFunctionClass(target, destination, Detours::X64Option::USE_REL32_CALL);
	}

	template <typename T>
	void __stdcall DetourClassJump(uintptr_t target, T destination)
	{
		Detours::X64::DetourFunctionClass(target, destination, Detours::X64Option::USE_REL32_JUMP);
	}
}