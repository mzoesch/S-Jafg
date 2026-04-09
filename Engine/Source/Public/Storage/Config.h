// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

struct LConfig final
{
    typedef std::unordered_map<LString, LString> Entries;
    typedef std::unordered_map<LString, Entries> Sections;

    inline ~LConfig()
    {
        this->PushAllConfigFiles();
    }

    //# Only pulls the config file if it was not pulled before.
    inline void PullConfigFile(LPath const& Path)
    {
        if (this->Map.contains(Path) == false)
        {
            this->ForcePullConfigFile(Path);
        }
    }
    //#
    //# Always pulls the config file and overwrites any existing values.
    //# Values that exists here but on in the file will *not* be erased.
    //#
    ENGINE_API void ForcePullConfigFile(LPath const& Path);

    inline void PushAllConfigFiles()
    {
        for (auto const& Path: this->Map | std::views::keys)
        {
            this->PushConfigFile(Path);
        }
    }
    ENGINE_API void PushConfigFile(LPath const& Path);

    inline void SetConfigValue(LPath const& Path, LString const& Section, LString const& Key, LString Value)
    {
        this->Map[Path][Section][Key] = std::move(Value);
    }
    inline TOptional<LString> GetConfigValue(LPath const& Path, LString const& Section, LString const& Key) const
    {
        if (auto It1{this->Map.find(Path)}; It1 != this->Map.end())
        {
            if (auto It2{It1->second.find(Section)}; It2 != It1->second.end())
            {
                if (auto It3{It2->second.find(Key)}; It3 != It2->second.end())
                {
                    return It3->second;
                }
            }
        }
        return {};
    }

    inline void AddConfigSection(LPath const& Path, LString const& Section, Entries const& Entries) noexcept
    {
        for (auto& Cur{this->Map[Path][Section]}; auto const& [Key, Value] : Entries)
        {
            Cur[Key] = Value;
        }
    }
    inline Entries* GetConfigSection(LPath const& Path, LString const& Section) noexcept
    {
        if (auto It1{this->Map.find(Path)}; It1 != this->Map.end())
        {
            if (auto It2{It1->second.find(Section)}; It2 != It1->second.end())
            {
                return &It2->second;
            }
        }
        return nullptr;
    }

private:

    std::unordered_map<LPath, Sections> Map;
};

} /* ~Namespace Jafg */
