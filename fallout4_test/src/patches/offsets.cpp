#include "../common.h"

#include <map>

std::unordered_map<uint64_t, uintptr_t> AddressMap;

namespace Offsets
{
	uintptr_t Resolve(uint32_t RelOffset, uint32_t Version)
	{
		return AddressMap.at(OFFSET_ENTRY_KEY(RelOffset, Version));
	}

	bool CanResolve(uint32_t RelOffset, uint32_t Version)
	{
		return Resolve(RelOffset, Version) != 0;
	}

	void BuildTableForCKF4Version(uint32_t Version)
	{
		Assert(false);
	}

	void BuildTableForCKSSEVersion(uint32_t Version)
	{
	}

	void BuildTableForGameVersion(uint32_t Version)
	{
		Assert(false);
	}

	void BuildTable(const OffsetEntry *Table, size_t Count, bool CurrentVersion)
	{
#if 0
		ValidateTable(Table, Count);
#endif

		for (size_t i = 0; i < Count; i++)
		{
			auto& entry = Table[i];

			auto key = OFFSET_ENTRY_KEY(entry.RelOffset, entry.Version);
			uintptr_t finalAddress = 0;

			if (AddressMap.count(key) > 0)
				continue;

			// If this table is for the current exe version, hardcoded translated addresses can be used
			if (CurrentVersion && entry.TranslatedOffset != -1)
				finalAddress = g_ModuleBase + entry.TranslatedOffset;

			// Try a signature scan instead
			if (!finalAddress && entry.Signature)
			{
				auto results = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, entry.Signature);

				if (!results.empty())
				{
					AssertMsgVa(results.size() <= 1, "Signatures are never supposed to have multiple results (0x%X, %lld results)", entry.RelOffset, results.size());

					finalAddress = results[0] + entry.SigAdjustment;
				}
			}

			// Addresses that can't be found are not an error. Marked as 0.
			AddressMap.try_emplace(key, finalAddress);
		}
	}

	void ValidateTable(const OffsetEntry *Table, size_t Count)
	{
		// If a signature is given, it should match the hardcoded address
		for (size_t i = 0; i < Count; i++)
		{
			auto& entry = Table[i];

			if (!entry.Signature)
				continue;

			auto results = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, entry.Signature);

			Assert(results.size() == 1);

			uintptr_t scanAddr = results[0] + entry.SigAdjustment;
			uintptr_t realAddr = g_ModuleBase + entry.TranslatedOffset;

			AssertMsgVa(scanAddr == realAddr, "0x%llX != 0x%llX", scanAddr, realAddr);
		}
	}

	void DumpLoadedTable(const char *FilePath)
	{
		FILE *out = fopen(FilePath, "w");

		if (out)
		{
			std::map sorted(AddressMap.begin(), AddressMap.end());

			for (auto& entry : sorted)
			{
				uint32_t ver = entry.first >> 32;
				uint32_t rel = entry.first & 0xFFFFFFFF;

				if (!entry.second)
					fprintf(out, "OFFSET_ENTRY(0x%X, %d, nullptr, 0, -1)// Bad entry\n", rel, ver);
				else
					fprintf(out, "OFFSET_ENTRY(0x%X, %d, nullptr, 0, 0x%llX)\n", rel, ver, entry.second - g_ModuleBase);
			}

			fclose(out);
		}
	}
}