#pragma once

enum TemplateDeletionCascade {
    // if specified, pages associated with a deleted template will continue to exist but will no longer have the
    // properties supplied by the template
    TemplateDeletionCascade_Detach,
    // if specified, pages associated with a deleted template will also be deleted
    TemplateDeletionCascade_Cascade,
    // if specified, pages associated with a deleted template will have a manual copy created for each of the properties
    // supplied by the template
    TemplateDeletionCascade_Retain,
};