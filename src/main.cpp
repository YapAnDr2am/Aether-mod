#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/GJGameLevel.hpp>

// Подключаем API для сохранения данных уровня
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>

using namespace geode::prelude;

// Структура, описывающая одну смерть
struct DeathPoint {
    float x, y;          // Координаты смерти
    int attempt;         // Номер попытки
    std::string cause;   // Причина (столкновение или падение)
};

// Главный класс мода. Мы "модифицируем" PlayLayer, чтобы перехватывать его методы.
class $modify(AetherPlayLayer, PlayLayer) {
    struct Fields {
        std::vector<DeathPoint> m_deathLog;
        std::map<int, int> m_deathHeatmap;
        CCDrawNode* m_heatmapNode = nullptr;
        bool m_adviceShownThisSegment = false;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        m_fields->m_heatmapNode = CCDrawNode::create();
        this->addChild(m_fields->m_heatmapNode, 100);

        auto savedLog = alpha::level_storage::getSavedValue<std::vector<DeathPoint>>(this, "death_log", Mod::get());
        if (savedLog) {
            m_fields->m_deathLog = *savedLog;
            for (const auto& dp : m_fields->m_deathLog) {
                int segment = static_cast<int>(dp.x / 100.0f);
                m_fields->m_deathHeatmap[segment]++;
            }
        }

        return true;
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        DeathPoint dp;
        dp.x = player->getPositionX();
        dp.y = player->getPositionY();
        dp.attempt = m_attempts;
        dp.cause = object ? "collision" : "fall";
        m_fields->m_deathLog.push_back(dp);

        int segment = static_cast<int>(dp.x / 100.0f);
        m_fields->m_deathHeatmap[segment]++;

        alpha::level_storage::setSavedValue(this, "death_log", m_fields->m_deathLog, Mod::get());

        m_fields->m_adviceShownThisSegment = false;

        PlayLayer::destroyPlayer(player, object);

        if (m_fields->m_deathHeatmap[segment] >= 5 && !m_fields->m_adviceShownThisSegment) {
            auto advice = getAdviceForSegment(segment);
            if (!advice.empty()) {
                Notification::create(advice, NotificationIcon::Info)->show();
                m_fields->m_adviceShownThisSegment = true;
            }
        }
    }

    void draw() {
        PlayLayer::draw();

        if (!m_fields->m_heatmapNode) return;

        m_fields->m_heatmapNode->clear();

        for (auto& [segment, count] : m_fields->m_deathHeatmap) {
            if (count < 3) continue;

            float intensity = std::min(1.0f, count / 10.0f);
            auto color = ccColor4F{1.0f, 0.0f, 0.0f, intensity * 0.3f};

            auto rect = CCRect(segment * 100.0f, 0, 100.0f, 320.0f);
            m_fields->m_heatmapNode->drawRect(rect, color, 0, color);
        }
    }

    std::string getAdviceForSegment(int segment) {
        return fmt::format("Segment {}: {} deaths. Try adjusting your jump timing.", 
                           segment, m_fields->m_deathHeatmap[segment]);
    }
};
