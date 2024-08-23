#ifndef EDITOR_CONSTANTSEDITOR_HPP
#define EDITOR_CONSTANTSEDITOR_HPP

namespace editor
{
/**
 * @brief Collection of constants specific in nature or value to the editor
 *
 * @ingroup Editor
 */
struct ConstantsEditor {
    static constexpr const char* ConfigFilePath = "editor.cfg";
    static constexpr unsigned int WindowWidth   = 800;
    static constexpr unsigned int WindowHeight  = 600;
};

} // namespace editor

#endif
