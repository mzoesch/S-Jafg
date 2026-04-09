// Copyright mzoesch. All rights reserved.

#include "Storage/Config.h"

//# An iterator over a string that yields each line as a string view.

enum struct line_iterator_behavior
{
    Primitive,
    CheckForEscapeSequences,
};

template<line_iterator_behavior Behavior>
struct line_iterator
{
    static inline constexpr char LineBreak{'\n'};

    typedef std::string_view value_type;
    typedef value_type::difference_type difference_type;
    typedef std::input_iterator_tag iterator_category;
    typedef std::input_iterator_tag iterator_concept;

    constexpr line_iterator() noexcept = default;
    constexpr line_iterator(std::string_view InString, std::size_t Start) noexcept
        : String(InString), Position(Start)
    {
        this->advance();
    }

    value_type operator*() const noexcept { return this->Current; }

    line_iterator& operator++() noexcept
    {
        this->Position = this->Next;
        this->advance();
        return *this;
    }

    constexpr bool operator==(line_iterator const& Rhs) const noexcept
    {
        return this->Position == Rhs.Position
            && this->String.data() == Rhs.String.data()
            && this->String.size() == Rhs.String.size();
    }

private:

    constexpr void advance() noexcept
    {
        if (this->Position >= this->String.size())
        {
            this->Position = this->String.size();
            return;
        }

        std::size_t End;
        if constexpr (Behavior == line_iterator_behavior::Primitive)
        {
            End = this->String.find(line_iterator::LineBreak, this->Position);
        }
        else if constexpr (Behavior == line_iterator_behavior::CheckForEscapeSequences)
        {
            bool bEscaped{};
            for (End = this->Position; End < this->String.size(); ++End)
            {
                if (this->String[End] == line_iterator::LineBreak && bEscaped == false)
                {
                    break;
                }
                bEscaped = this->String[End] == '\\' && bEscaped == false;
            }
            if (End == this->String.size())
            {
                End = std::string_view::npos;
            }
        }
        else
        {
            static_assert(sizeof(std::size_t) == 0);
        }

        if (End == std::string_view::npos)
        {
            this->Current = this->String.substr(this->Position);
            this->Next = this->String.size();
        }
        else
        {
            this->Current = this->String.substr(this->Position, End - this->Position);
            this->Next = End + 1;
        }

        return;
    }

    std::string_view String;
    std::string_view Current;
    std::size_t Position{};
    std::size_t Next{};
};

template<line_iterator_behavior Behavior>
struct over_lines
{
    explicit constexpr over_lines(std::string_view String) noexcept
        : String(String)
    {
    }

    line_iterator<Behavior> begin() const noexcept { return line_iterator<Behavior>{this->String, 0}; }
    line_iterator<Behavior> end() const noexcept { return line_iterator<Behavior>{this->String, this->String.size()}; }

private:

    std::string_view String;
};

void Jafg::LConfig::ForcePullConfigFile(LPath const& Path)
{
    if (Finder::DoesFileExist(Path) == false)
    {
        this->Map[Path];
        return;
    }

    auto Content{Finder::ReadFile(Path)};

    Sections ImportedSections;
    Entries* CurrentEntries{};

    auto Line{0uz};
    for (std::string_view LineStrView : over_lines<line_iterator_behavior::CheckForEscapeSequences>{Content})
    {
        ++Line;

        if (LineStrView.starts_with('['))
        {
            if (LineStrView.ends_with(']') == false)
            {
                LOG_FATAL(LogConfigIo, "Malformed config file [{}]. Section header does not end with ']'. Line: [{}]", Path, Line)
            }
            LineStrView = LineStrView.substr(1, LineStrView.size() - 2);
            if (LineStrView.empty())
            {
                LOG_FATAL(LogConfigIo, "Malformed config file [{}]. Section header is empty. Line: [{}]", Path, Line)
            }
            LString LineStr{LineStrView};
            if (ImportedSections.contains(LineStr))
            {
                LOG_FATAL(LogConfigIo, "Malformed config file [{}]. Section header [{}] is duplicated. Line: [{}]", Path, LineStr, Line)
            }

            ImportedSections[LineStr];
            CurrentEntries = &ImportedSections[LineStr];
            continue;
        }

        auto EqualSignIdx{LineStrView.find('=')};
        if (EqualSignIdx == std::string_view::npos)
        {
            if (LineStrView == "\n")
            {
                continue;
            }
            LOG_FATAL(LogConfigIo, "Malformed config file [{}]. No '=' found in entry. Line: [{}]", Path, Line)
        }

        if (CurrentEntries == nullptr)
        {
            LOG_FATAL(LogConfigIo, "Malformed config file [{}]. No section header found before entry. Line: [{}]", Path, Line)
        }

        LString Key{LineStrView.substr(0, EqualSignIdx)};
        if (Key.empty())
        {
            LOG_FATAL(LogConfigIo, "Malformed config file [{}]. Key is empty. Line: [{}]", Path, Line)
        }

        LString Value;
        bool bEscaped{};
        for (char C : LineStrView.substr(EqualSignIdx + 1))
        {
            if (bEscaped)
            {
                if (C == 'n')
                {
                    Value += '\n';
                }
                else if (C == '\\')
                {
                    Value += '\\';
                }
                else
                {
                    LOG_FATAL(LogConfigIo, "Malformed config file [{}]. Invalid escape sequence '\\{}'. Line: [{}]", Path, C, Line)
                }
                bEscaped = false;
            }
            else if (C == '\\')
            {
                bEscaped = true;
            }
            else
            {
                Value += C;
            }
            continue;
        }
        jassert(bEscaped == false)

        if (CurrentEntries->contains(Key))
        {
            LOG_FATAL(LogConfigIo
                , "Malformed config file [{}]. Key [{}] is a duplicate. Line: [{}]"
                , Path, Key, Line
                )
        }
        (*CurrentEntries)[Key] = std::move(Value);
    }

    if (ImportedSections.empty() == false)
    {
        this->Map[Path] = std::move(ImportedSections);
    }

    return;
}

void Jafg::LConfig::PushConfigFile(LPath const& Path)
{
    if (Finder::DoesFileExist(Path) == false)
    {
        Finder::CreateFile(Path);
    }

    std::stringstream Content;
    for (auto const& [Section, Entries] : this->Map[Path])
    {
        check( Section.contains('\\') == false
            && Section.contains('\n') == false
            && Section.contains('[')  == false
            && Section.contains(']')  == false
            )
        Content << "[" << Section << "]\n";
        for (auto const& [Key, Value] : Entries)
        {
            check( Key.contains('\\') == false
                && Key.contains('\n') == false
                && Key.contains('[')  == false
                && Key.contains(']')  == false
                )
            Content << Key << "=";
            for (char C : Value)
            {
                if (C == '\n')
                {
                    Content << "\\n";
                }
                else if (C == '\\')
                {
                    Content << "\\\\";
                }
                else
                {
                    Content << C;
                }
            }
            Content << "\n";
        }
    }

    Finder::OverrideFile(Path, Content.str());

    return;
}
